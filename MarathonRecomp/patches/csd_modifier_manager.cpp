#include "csd_modifier_manager.h"
#include <os/logger.h>
#include <patches/aspect_ratio_patches.h>
#include <ui/imgui_utils.h>
#include <user/config.h>
#include <app.h>
#include <nfd.h>
#include <xxHashMap.h>

static Mutex g_Mutex{};
static xxHashMap<CsdModifier> g_Modifiers{};
static std::map<Chao::CSD::Project*, ptrdiff_t> g_Projects{};
static std::map<const void*, XXH64_hash_t> g_Paths{};

void EmplacePath(Chao::CSD::Project* in_pProject, const void* key, const std::string_view& value)
{
    std::lock_guard lock(g_Mutex);

    const auto hash = HashStr(value);
    g_Paths.emplace(key, hash);

    auto length = ptrdiff_t(key) - ptrdiff_t(in_pProject);

    if (const auto it = g_Projects.find(in_pProject); it != g_Projects.end())
        length = std::max(it->second, length);

    g_Projects[in_pProject] = length;

#ifdef _DEBUG
    printf("[*] CSD hierarchy: %s (0x%llx) (0x%llx)\n", value.data(), hash, uintptr_t(key));
#endif
}

void TraverseCast(Chao::CSD::Project* in_pProject, Chao::CSD::Scene* scene, uint32_t castNodeIndex, Chao::CSD::CastNode* castNode, uint32_t castIndex, const std::string& parentPath)
{
    if (castIndex == ~0)
        return;

    TraverseCast(in_pProject, scene, castNodeIndex, castNode, castNode->pCastLinks[castIndex].SiblingCastIndex, parentPath);

    std::string path = parentPath;

    for (size_t i = 0; i < scene->CastCount; i++)
    {
        auto& index = scene->pCastIndices[i];

        if (index.CastNodeIndex == castNodeIndex && index.CastIndex == castIndex)
        {
            path += index.pCastName;
            break;
        }
    }

    EmplacePath(in_pProject, castNode->pCasts[castIndex].get(), path);

    if (castNode->RootCastIndex == castIndex)
        EmplacePath(in_pProject, castNode, path);

    path += "/";

    TraverseCast(in_pProject, scene, castNodeIndex, castNode, castNode->pCastLinks[castIndex].ChildCastIndex, path);
}

void TraverseScene(Chao::CSD::Project* in_pProject, Chao::CSD::Scene* scene, std::string path)
{
    EmplacePath(in_pProject, scene, path);

    path += "/";

    for (size_t i = 0; i < scene->CastNodeCount; i++)
    {
        auto& castNode = scene->pCastNodes[i];
        TraverseCast(in_pProject, scene, i, &castNode, castNode.RootCastIndex, path);
    }
}

void TraverseSceneNode(Chao::CSD::Project* in_pProject, Chao::CSD::SceneNode* sceneNode, std::string path)
{
    EmplacePath(in_pProject, sceneNode, path);

    path += "/";

    for (size_t i = 0; i < sceneNode->SceneCount; i++)
    {
        auto& sceneIndex = sceneNode->pSceneIndices[i];
        TraverseScene(in_pProject, sceneNode->pScenes[sceneIndex.SceneIndex], path + sceneIndex.pSceneName.get());
    }

    for (size_t i = 0; i < sceneNode->SceneNodeCount; i++)
    {
        auto& sceneNodeIndex = sceneNode->pSceneNodeIndices[i];
        TraverseSceneNode(in_pProject, &sceneNode->pSceneNodes[sceneNodeIndex.SceneNodeIndex], path + sceneNodeIndex.pSceneNodeName.get());
    }
}

// Sonicteam::CsdResource::Create
PPC_FUNC_IMPL(__imp__sub_82617570);
PPC_FUNC(sub_82617570)
{
    const auto pFilePath = reinterpret_cast<char*>(base + PPC_LOAD_U32(ctx.r4.u32 + 4));

    __imp__sub_82617570(ctx, base);

    if (!ctx.r3.u32)
        return;

    const auto ppCsdObject = PPC_LOAD_U32(ctx.r3.u32);

    if (!ppCsdObject)
        return;

    const auto pCsdObject = reinterpret_cast<Sonicteam::CsdObject*>(base + ppCsdObject);

    if (!pCsdObject)
        return;

    const auto pCsdProject = pCsdObject->m_pCsdProject.get();

    if (!pCsdProject)
        return;

    LOGFN_UTILITY("CSD loaded: {} (0x{:08X})", pFilePath, uintptr_t(pCsdProject));

    static const char* languages[7] =
    {
        nullptr, // Maps to ELanguages enum.
        "_English",
        "_Japanese",
        "_German",
        "_French",
        "_Spanish",
        "_Italian"
    };

    const auto filePathLen = strlen(pFilePath);
    const auto pSuffix = languages[int(Config::Language.Value)];
    const auto suffixLen = strlen(pSuffix);

    // Truncate language names to redirect CSD modifiers.
    if (suffixLen < filePathLen)
    {
        if (strcmpIgnoreCase(pFilePath + filePathLen - suffixLen, pSuffix))
        {
            pFilePath[filePathLen - suffixLen] = '\0';

            LOGFN_UTILITY("CSD modifier(s) redirected: {}", pFilePath);
        }
    }

    TraverseSceneNode(pCsdProject->m_pResource, pCsdProject->m_pResource->pRootNode, pFilePath);
}

// Chao::CSD::CProject::~CProject
PPC_FUNC_IMPL(__imp__sub_825CE5F0);
PPC_FUNC(sub_825CE5F0)
{
    const auto pProject = reinterpret_cast<Chao::CSD::CProject*>(base + ctx.r3.u32);

    std::lock_guard lock(g_Mutex);

    const auto key = pProject->m_pResource.get();

    if (const auto it = g_Projects.find(key); it != g_Projects.end())
    {
        const auto length = g_Projects[key];
        const auto end = reinterpret_cast<uint8_t*>(key) + length;

        const auto lower = g_Paths.lower_bound(key);
        const auto upper = g_Paths.upper_bound(end);

        g_Paths.erase(lower, upper);
        g_Projects.erase(key);

        LOGFN_UTILITY("CSD freed: 0x{:08X} - 0x{:08X}", uintptr_t(key), uintptr_t(end));
    }
    else
    {
        LOGFN_ERROR("CSD leaked: 0x{:08X}", uintptr_t(key));
    }

    __imp__sub_825CE5F0(ctx, base);
}

bool CsdModifierManager::Read(const nlohmann::json& in_rReader)
{
    for (const auto& item : in_rReader)
    {
        for (const auto& [rPath, rModifier] : item.items())
        {
            auto modifier = CsdModifier();
            
            if (!modifier.Read(rModifier))
                continue;

            modifier.Path = rPath;
            
            g_Modifiers[HashStr(rPath)] = modifier;
        }
    }

    return true;
}

bool CsdModifierManager::Write(const std::filesystem::path& in_rPath)
{
    auto file = std::ofstream(in_rPath);

    if (!file)
        return false;

    nlohmann::json json{};

    for (auto& rModifier : g_Modifiers)
        rModifier.second.Write(json[rModifier.second.Path]);

    file << json.dump(4) << std::endl;

    return true;
}

CsdModifier* CsdModifierManager::Find(uint32_t in_data)
{
    std::lock_guard lock(g_Mutex);

    const auto path = g_Paths.find(g_memory.Translate(in_data));

    if (path != g_Paths.end())
    {
        const auto hash = path->second;
        const auto modifier = g_Modifiers.find(hash);

        if (modifier != g_Modifiers.end())
            return &modifier->second;
    }

    return nullptr;
}

void CsdModifierManager::DrawDebug()
{
    if (!App::s_pApp)
        return;

    static bool isVisible{};
    static bool wasVisible{};

    if (ImGui::IsKeyPressed(ImGuiKey_F4))
        isVisible = !isVisible;

    if (isVisible)
    {
        App::s_pApp->m_pDoc->SetPause(true);
        wasVisible = true;
    }
    else
    {
        if (wasVisible)
        {
            App::s_pApp->m_pDoc->SetPause(false);
            wasVisible = false;
        }

        return;
    }

    const auto pFont = g_pFntRodin;
    const auto defaultFontScale = pFont->Scale;
    pFont->Scale = ImGui::GetDefaultFont()->FontSize / pFont->FontSize;
    ImGui::PushFont(pFont);

    std::vector<const void*> deletions{};

    static bool hasSetWindowSize{};

    if (!hasSetWindowSize)
    {
        ImGui::SetNextWindowSize({ 733.0f, 775.0f });
        hasSetWindowSize = true;
    }

    if (ImGui::Begin("CSD Debug", &isVisible))
    {
        if (ImGui::CollapsingHeader("Configuration", ImGuiTreeNodeFlags_DefaultOpen))
        {
            static std::array<const char*, 2> alignmentModes = { "Edge", "Center" };
            ImGui::Combo("Alignment Mode", reinterpret_cast<int*>(&Config::UIAlignmentMode.Value), alignmentModes.data(), alignmentModes.size());

            ImGui::Checkbox("Disable Lives", &Config::DisableLives.Value);
        }

        static int stepCount{};
        static float stepDelay{};
        static float stepTime{};

        if (ImGui::CollapsingHeader("Playback", ImGuiTreeNodeFlags_DefaultOpen))
        {
            static int stepCountQueue{};
            stepCountQueue = std::clamp(stepCountQueue, 1, INT_MAX);

            ImGui::DragInt("Step Count", &stepCountQueue, 1.0f, 1, INT_MAX);
            ImGui::DragFloat("Step Delay", &stepDelay, 0.01f, 0.0f, FLT_MAX);

            const char* stepButtonLabel = "Step";

            if (stepCount)
                stepButtonLabel = "Stop";
            else if (stepCountQueue > 1)
                stepButtonLabel = "Play";

            if (ImGui::Button(stepButtonLabel, { ImGui::GetContentRegionAvail().x, 0.0f }))
            {
                stepCount = stepCount ? 0 : stepCountQueue;
                stepTime = 0;
            }
        }

        if (stepCount)
        {
            if (stepTime >= stepDelay)
            {
                App::s_pApp->m_pDoc->SetPause(false);
                stepCount--;
                stepTime = 0;
            }
            else
            {
                stepTime += ImGui::GetIO().DeltaTime;
            }
        }

        if (ImGui::CollapsingHeader("Modifiers", ImGuiTreeNodeFlags_DefaultOpen))
        {
            static size_t modifierCount{};

            if (ImGui::BeginTable("##Diagnostics", 2))
            {
                size_t projectsLength{};

                for (const auto& [rProject, rSize] : g_Projects)
                    projectsLength += rSize + sizeof(Chao::CSD::Project);

                IMGUI_GENERIC_ROW("Projects", "%zu (%.2f KiB)", g_Projects.size(), static_cast<double>(projectsLength / 1024.0));
                IMGUI_GENERIC_ROW("Nodes", "%zu", g_Paths.size());
                IMGUI_GENERIC_ROW("Modifiers", "%zu (%.2f KiB)", modifierCount, static_cast<double>(modifierCount * sizeof(CsdModifier) / 1024.0));
                ImGui::EndTable();
            }

            modifierCount = 0;

            const auto expandAll = ImGui::Button("Expand All", { ImGui::GetContentRegionAvail().x * 0.5f, 0.0f });
            ImGui::SameLine();
            const auto collapseAll = ImGui::Button("Collapse All", { ImGui::GetContentRegionAvail().x, 0.0f });
            ImGui::Separator();

            const auto footerHeight = ImGui::GetFrameHeight() + ImGui::GetStyle().ItemSpacing.y + 4.0f;

            if (ImGui::BeginChild("##Modifiers", { 0, -footerHeight }))
            {
                for (const auto& rElement : g_Paths)
                {
                    const auto addr = g_memory.MapVirtual(rElement.first);
                    const auto modifier = Find(addr);

                    if (!modifier)
                        continue;

                    modifierCount++;

                    ImGui::PushID(addr);

                    if (expandAll)
                        ImGui::SetNextItemOpen(true);
                    else if (collapseAll)
                        ImGui::SetNextItemOpen(false);

                    if (ImGui::CollapsingHeader(std::format("{} (0x{:X})", modifier->Path, uintptr_t(rElement.first)).c_str()))
                    {
                        const auto tableFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY;

                        if (ImGui::TreeNodeEx("Flags", ImGuiTreeNodeFlags_DefaultOpen))
                        {
                            if (ImGui::BeginTable("##Flags", g_CsdFlagNames.size() + 1, tableFlags, { 0, 398.0f }))
                            {
                                ImGui::TableSetupScrollFreeze(1, 0);
                                ImGui::TableSetupColumn("");

                                for (const auto& [rFlag, rName] : g_CsdFlagNames)
                                    ImGui::TableSetupColumn(rName, ImGuiTableColumnFlags_AngledHeader);

                                ImGui::TableAngledHeadersRow();
                                ImGui::TableNextRow();

                                const auto createFlagsControls = [](const char* in_pLabel, uint64_t* in_pFlags)
                                {
                                    ImGui::TableNextRow();
                                    ImGui::TableNextColumn();
                                    ImGui::AlignTextToFramePadding();
                                    ImGui::Text("%s", in_pLabel);

                                    for (const auto& [rFlag, rName] : g_CsdFlagNames)
                                    {
                                        ImGui::TableNextColumn();
                                        ImGui::PushID(std::format("{}##{}", in_pLabel, rName).c_str());
                                        ImGui::CheckboxFlags("", in_pFlags, rFlag);
                                        ImGui::PopID();
                                    }
                                };

                                createFlagsControls("Flags", &modifier->Flags);
                                createFlagsControls("Ultrawide Mask", &modifier->Ultrawide.Mask);
                                createFlagsControls("Ultrawide Edge Mask", &modifier->Ultrawide.Edge.Mask);
                                createFlagsControls("Ultrawide Center Mask", &modifier->Ultrawide.Center.Mask);
                                createFlagsControls("Narrow Mask", &modifier->Narrow.Mask);
                                createFlagsControls("Narrow Edge Mask", &modifier->Narrow.Edge.Mask);
                                createFlagsControls("Narrow Center Mask", &modifier->Narrow.Center.Mask);

                                ImGui::EndTable();
                            }

                            if (ImGui::TreeNode("Raw"))
                            {
                                const auto createRawFlagsControls = [&](const char* in_pLabel, const uint64_t in_flags)
                                {
                                    if (ImGui::TreeNode(in_pLabel))
                                    {
                                        const auto value = std::format("0x{:016X}", in_flags);
                                        const auto bits = std::format("{:#064b}", in_flags);

                                        if (ImGui::BeginTable("##FlagInfo", 2))
                                        {
                                            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
                                            IMGUI_GENERIC_ROW("Value", "%s", value.c_str());
                                            IMGUI_GENERIC_ROW("Bits", "%s", bits.c_str());
                                            ImGui::EndTable();
                                        }

                                        if (ImGui::Button("Copy Value"))
                                            ImGui::SetClipboardText(value.c_str());

                                        ImGui::SameLine();

                                        if (ImGui::Button("Copy Bits"))
                                            ImGui::SetClipboardText(bits.c_str());

                                        ImGui::TreePop();
                                    }
                                };

                                createRawFlagsControls("Flags", modifier->Flags);
                                createRawFlagsControls("Ultrawide Mask", modifier->Ultrawide.Mask);
                                createRawFlagsControls("Ultrawide Edge Mask", modifier->Ultrawide.Edge.Mask);
                                createRawFlagsControls("Ultrawide Center Mask", modifier->Ultrawide.Center.Mask);
                                createRawFlagsControls("Narrow Mask", modifier->Narrow.Mask);
                                createRawFlagsControls("Narrow Edge Mask", modifier->Narrow.Edge.Mask);
                                createRawFlagsControls("Narrow Center Mask", modifier->Narrow.Center.Mask);

                                ImGui::TreePop();
                            }

                            ImGui::TreePop();
                        }

                        if (ImGui::TreeNode("Alignment"))
                        {
                            const auto dragRgba8 = [](const char* in_pLabel, Sonicteam::SoX::RGBA8* in_pColor, const float in_speed = 1.0f)
                            {
                                ImGui::PushID(in_pLabel);
                                ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(2.0f, 0.0f));

                                if (ImGui::BeginTable("##RGBA", 5))
                                {
                                    const uint8_t min = 0;
                                    const uint8_t max = 255;

                                    ImGui::TableNextColumn();
                                    ImGui::SetNextItemWidth(-FLT_MIN);
                                    ImGui::DragScalar("##R", ImGuiDataType_U8, &in_pColor->Data[0].value, 1.0f, &min, &max);
                                    ImGui::TableNextColumn();
                                    ImGui::SetNextItemWidth(-FLT_MIN);
                                    ImGui::DragScalar("##G", ImGuiDataType_U8, &in_pColor->Data[1].value, 1.0f, &min, &max);
                                    ImGui::TableNextColumn();
                                    ImGui::SetNextItemWidth(-FLT_MIN);
                                    ImGui::DragScalar("##B", ImGuiDataType_U8, &in_pColor->Data[2].value, 1.0f, &min, &max);
                                    ImGui::TableNextColumn();
                                    ImGui::SetNextItemWidth(-FLT_MIN);
                                    ImGui::DragScalar("##A", ImGuiDataType_U8, &in_pColor->Data[3].value, 1.0f, &min, &max);
                                    ImGui::TableNextColumn();
                                    ImGui::AlignTextToFramePadding();
                                    ImGui::Text("%s", in_pLabel);
                                    ImGui::TableNextColumn();
                                    ImGui::EndTable();
                                }

                                ImGui::PopStyleVar();
                                ImGui::PopID();
                            };

                            const auto drawPositionModifierNode = [](const char* in_pLabel, CsdVertexModifierGroup* in_pModifier, float in_speed = 1.0f)
                            {
                                if (ImGui::TreeNode(in_pLabel))
                                {
                                    ImGui::DragFloat2("Top Left", &in_pModifier->TL.X, in_speed);
                                    ImGui::DragFloat2("Bottom Left", &in_pModifier->BL.X, in_speed);
                                    ImGui::DragFloat2("Top Right", &in_pModifier->TR.X, in_speed);
                                    ImGui::DragFloat2("Bottom Right", &in_pModifier->BR.X, in_speed);
                                    ImGui::DragFloat2("All", &in_pModifier->Offset.X, in_speed);
                                    ImGui::DragFloat2("Extend Bias", &in_pModifier->ExtendBias.X, 0.01f);
                                    ImGui::TreePop();
                                }
                            };

                            const auto drawUvModifierNode = [](const char* in_pLabel, CsdVertexModifierGroup* in_pModifier, float in_speed = 0.01f)
                            {
                                if (ImGui::TreeNode(in_pLabel))
                                {
                                    ImGui::DragFloat2("Top Left", &in_pModifier->TL.U, in_speed);
                                    ImGui::DragFloat2("Bottom Left", &in_pModifier->BL.U, in_speed);
                                    ImGui::DragFloat2("Top Right", &in_pModifier->TR.U, in_speed);
                                    ImGui::DragFloat2("Bottom Right", &in_pModifier->BR.U, in_speed);
                                    ImGui::TreePop();
                                }
                            };

                            const auto drawColorModifierNode = [&](const char* in_pLabel, CsdVertexModifierGroup* in_pModifier, float in_speed = 1.0f)
                            {
                                if (ImGui::TreeNode(in_pLabel))
                                {
                                    dragRgba8("Top Left", &in_pModifier->TL.Color, in_speed);
                                    dragRgba8("Bottom Left", &in_pModifier->BL.Color, in_speed);
                                    dragRgba8("Top Right", &in_pModifier->TR.Color, in_speed);
                                    dragRgba8("Bottom Right", &in_pModifier->BR.Color, in_speed);
                                    ImGui::TreePop();
                                }
                            };

                            const auto drawModifiers = [&](CsdVertexModifierGroup* in_pModifier)
                            {
                                drawPositionModifierNode("Position Modifier", in_pModifier);
                                drawUvModifierNode("UV Modifier", in_pModifier);
                                drawColorModifierNode("Color Modifier", in_pModifier);
                            };

                            const auto drawVertexModifierGroupNode = [&](std::string_view in_name, CsdVertexModifierGroup* in_pModifier)
                            {
                                if (ImGui::TreeNode(in_name.data()))
                                {
                                    drawModifiers(in_pModifier);
                                    ImGui::TreePop();
                                }
                            };

                            const auto drawAlignmentModifierNode = [&](std::string_view in_name, CsdAlignmentModifier* in_pModifier)
                            {
                                if (ImGui::TreeNode(in_name.data()))
                                {
                                    drawVertexModifierGroupNode("Base", &in_pModifier->Base);
                                    drawVertexModifierGroupNode("Repeat", &in_pModifier->Repeat);
                                    ImGui::TreePop();
                                }
                            };

                            const auto drawAlignmentModifierGroupNode = [&](std::string_view in_name, CsdAlignmentModifierGroup* in_pModifier)
                            {
                                if (ImGui::TreeNode(in_name.data()))
                                {
                                    drawAlignmentModifierNode("Edge", &in_pModifier->Edge);
                                    drawAlignmentModifierNode("Center", &in_pModifier->Center);
                                    ImGui::TreePop();
                                }
                            };

                            drawAlignmentModifierGroupNode("Ultrawide", &modifier->Ultrawide);
                            drawAlignmentModifierGroupNode("Narrow", &modifier->Narrow);

                            ImGui::TreePop();
                        }

                        if (ImGui::TreeNode("Corners"))
                        {
                            if (ImGui::BeginTable("##Corners", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
                            {
                                for (int i = 0; i < 8; i++)
                                {
                                    ImGui::TableNextColumn();
                                    ImGui::AlignTextToFramePadding();
                                    ImGui::Text("%f", g_csdCorners[i]);
                                }

                                ImGui::EndTable();
                            }

                            ImGui::DragFloat("Corner Max", &modifier->CornerMax, 0.01f);
                            ImGui::SliderInt("Corner Index", &modifier->CornerIndex, 0, 7);

                            ImGui::TreePop();
                        }

                        ImGui::Separator();

                        if (ImGui::Button("Copy Address"))
                            ImGui::SetClipboardText(std::format("0x{:X}", uintptr_t(rElement.first)).c_str());

                        ImGui::SameLine();

                        if (ImGui::Button("Copy Definition"))
                            ImGui::SetClipboardText(modifier->ToString().c_str());

                        ImGui::SameLine();

                        if (ImGui::Button("Delete"))
                        {
                            ImGui::OpenPopup("Delete Modifier?");
                            ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
                        }

                        if (ImGui::BeginPopupModal("Delete Modifier?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
                        {
                            ImGui::Text("Are you sure you want to delete this modifier?\nThis action is irreversible until reboot.");
                            ImGui::Separator();

                            if (ImGui::Button("Yes"))
                            {
                                deletions.push_back(rElement.first);
                                ImGui::CloseCurrentPopup();
                            }

                            ImGui::SameLine();

                            if (ImGui::Button("No"))
                                ImGui::CloseCurrentPopup();

                            ImGui::EndPopup();
                        }
                    }

                    ImGui::PopID();
                }
            }

            ImGui::EndChild();
            ImGui::Separator();

            if (ImGui::Button("Export", { ImGui::GetContentRegionAvail().x, 0.0f }))
            {
                NFD_Init();

                static const nfdu8filteritem_t filter[1] =
                {
                    { "JSON", "json" }
                };

                static const nfdsavedialogu8args_t args
                {
                    .filterList = filter,
                    .filterCount = 1,
                    .defaultName = "csd_modifiers.json"
                };

                nfdu8char_t* savePath{};

                if (NFD_SaveDialogU8_With(&savePath, &args) == NFD_OKAY)
                {
                    Write(savePath);
                    NFD_FreePathU8(savePath);
                }

                NFD_Quit();
            }
        }
    }

    for (const auto& rElement : deletions)
        g_Paths.erase(rElement);

    deletions.clear();

    ImGui::End();
    ImGui::PopFont();

    pFont->Scale = defaultFontScale;
}
