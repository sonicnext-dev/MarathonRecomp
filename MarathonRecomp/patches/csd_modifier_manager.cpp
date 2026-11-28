#include "csd_modifier_manager.h"
#include <os/logger.h>
#include <patches/aspect_ratio_patches.h>
#include <res/csd_modifiers.json.h>
#include <ui/imgui_utils.h>
#include <user/config.h>
#include <app.h>
#include <decompressor.h>
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

CsdModifierManager::Status CsdModifierManager::Read(const nlohmann::json& in_rReader)
{
    if (!in_rReader.contains("Version"))
        return CsdModifierManager::Status::MissingVersion;

    const size_t version = in_rReader["Version"];

    if (version != Version)
        return CsdModifierManager::Status::UnsupportedVersion;

    if (!in_rReader.contains("Modifiers"))
        return CsdModifierManager::Status::MissingModifiers;

    for (const auto& [rPath, rModifier] : in_rReader["Modifiers"].items())
    {
        auto modifier = CsdModifier();
            
        if (!modifier.Read(rModifier))
            continue;

        modifier.Path = rPath;
            
        g_Modifiers[HashStr(rPath)] = modifier;
    }

    return CsdModifierManager::Status::Success;
}

CsdModifierManager::Status CsdModifierManager::Read(const std::filesystem::path& in_rPath)
{
    auto file = std::ifstream(in_rPath);

    if (!file)
        return CsdModifierManager::Status::IOError;

    nlohmann::json json{};

    try
    {
        json = nlohmann::json::parse(file);
    }
    catch (const nlohmann::json::parse_error& ex)
    {
        LOGFN_ERROR("{}", ex.what());
        return CsdModifierManager::Status::ParseError;
    }

    return Read(json);
}

CsdModifierManager::Status CsdModifierManager::Read()
{
    CsdModifierManager::Status result{};
    nlohmann::json json{};

    try
    {
        const auto data = decompressZstd(g_csd_modifiers, g_csd_modifiers_uncompressed_size);
        json = nlohmann::json::parse(data.get(), data.get() + g_csd_modifiers_uncompressed_size);
    }
    catch (const nlohmann::json::parse_error& ex)
    {
        LOGFN_ERROR("{}", ex.what());
        result = CsdModifierManager::Status::ParseError;
    }

    if (result == CsdModifierManager::Status::Success)
        result = CsdModifierManager::Read(json);

    if (result != CsdModifierManager::Status::Success)
        LOGF_IMPL(os::logger::ELogType::Error, "CSD Modifier Manager", "{}", CsdModifierManager::GetStatusMessage(result));

    return result;
}

CsdModifierManager::Status CsdModifierManager::Write(const std::filesystem::path& in_rPath)
{
    auto file = std::ofstream(in_rPath);

    if (!file)
        return CsdModifierManager::Status::IOError;

    nlohmann::json json{};

    json["Version"] = CsdModifierManager::Version;

    for (auto& rModifier : g_Modifiers)
        rModifier.second.Write(json["Modifiers"][rModifier.second.Path]);

    file << json.dump(4) << std::endl;

    return CsdModifierManager::Status::Success;
}

const char* CsdModifierManager::GetStatusMessage(const Status in_status)
{
    return CsdModifierManager::ms_StatusMessages[int(in_status)];
}

void CsdModifierManager::Add(const CsdModifier& in_rModifier)
{
    std::lock_guard lock(g_Mutex);
    g_Modifiers[HashStr(in_rModifier.Path)] = in_rModifier;
}

bool CsdModifierManager::Remove(const std::string_view in_key)
{
    std::lock_guard lock(g_Mutex);
    return g_Modifiers.erase(HashStr(in_key));
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

static bool CreateFlagsTable(const ImVec2& in_rSize = { 0, 0 })
{
    const auto tableFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY;
    const auto result = ImGui::BeginTable("##Flags", g_CsdFlagNames.size() + 1, tableFlags, in_rSize);

    if (!result)
        return result;

    ImGui::TableSetupScrollFreeze(1, 0);
    ImGui::TableSetupColumn("");

    for (const auto& [rFlag, rName] : g_CsdFlagNames)
        ImGui::TableSetupColumn(rName, ImGuiTableColumnFlags_AngledHeader);

    ImGui::TableAngledHeadersRow();

    return result;
}

static void CreateFlagsRow(const char* in_pLabel, uint64_t* in_pFlags)
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
}

static void DragRGBAF32(const char* in_pLabel, Sonicteam::SoX::RGBAF32* in_pColor, const float in_speed = 0.01f)
{
    ImGui::PushID(in_pLabel);
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(2.0f, 0.0f));

    if (ImGui::BeginTable("##RGBA", 5))
    {
        const float min = 0.0f;
        const float max = 1.0f;

        float r = in_pColor->Data[0].get();
        float g = in_pColor->Data[1].get();
        float b = in_pColor->Data[2].get();
        float a = in_pColor->Data[3].get();

        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::DragScalar("##R", ImGuiDataType_Float, &r, in_speed, &min, &max);
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::DragScalar("##G", ImGuiDataType_Float, &g, in_speed, &min, &max);
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::DragScalar("##B", ImGuiDataType_Float, &b, in_speed, &min, &max);
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::DragScalar("##A", ImGuiDataType_Float, &a, in_speed, &min, &max);
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", in_pLabel);
        ImGui::TableNextColumn();

        in_pColor->Data[0] = r;
        in_pColor->Data[1] = g;
        in_pColor->Data[2] = b;
        in_pColor->Data[3] = a;

        ImGui::EndTable();
    }

    ImGui::PopStyleVar();
    ImGui::PopID();
}

static void DrawPositionModifierNode(const char* in_pLabel, CsdVertexModifierGroup* in_pModifier, float in_speed = 1.0f)
{
    if (ImGui::TreeNode(in_pLabel))
    {
        ImGui::DragFloat2("Top Left", &in_pModifier->TL.X, in_speed);
        ImGui::DragFloat2("Bottom Left", &in_pModifier->BL.X, in_speed);
        ImGui::DragFloat2("Top Right", &in_pModifier->TR.X, in_speed);
        ImGui::DragFloat2("Bottom Right", &in_pModifier->BR.X, in_speed);
        ImGui::DragFloat2("All", &in_pModifier->Offset.X, in_speed);

        if (ImGui::TreeNode("Extend Bias"))
        {
            ImGui::DragFloat2("Top Left", &in_pModifier->Extend.TopLeft.X, 0.01f);
            ImGui::DragFloat2("Bottom Right", &in_pModifier->Extend.BottomRight.X, 0.01f);
            ImGui::TreePop();
        }

        ImGui::TreePop();
    }
}

static void DrawUVModifierNode(const char* in_pLabel, CsdVertexModifierGroup* in_pModifier, float in_speed = 0.01f)
{
    if (ImGui::TreeNode(in_pLabel))
    {
        ImGui::DragFloat2("Top Left", &in_pModifier->TL.U, in_speed);
        ImGui::DragFloat2("Bottom Left", &in_pModifier->BL.U, in_speed);
        ImGui::DragFloat2("Top Right", &in_pModifier->TR.U, in_speed);
        ImGui::DragFloat2("Bottom Right", &in_pModifier->BR.U, in_speed);
        ImGui::DragFloat2("All", &in_pModifier->Offset.U, in_speed);
        ImGui::TreePop();
    }
}

static void DrawColorModifierNode(const char* in_pLabel, CsdVertexModifierGroup* in_pModifier, float in_speed = 0.01f)
{
    if (ImGui::TreeNode(in_pLabel))
    {
        static std::array<const char*, 2> colorModes = { "Multiply", "Set" };

        const auto createColorControls = [&](const std::string_view in_name, CsdVertexModifier* in_pModifier)
        {
            ImGui::PushID(in_pModifier);

            ImGui::SetNextItemWidth(80.0f);
            ImGui::Combo("##Mode", reinterpret_cast<int*>(&in_pModifier->ColorMode), colorModes.data(), colorModes.size());

            ImGui::SameLine();

            DragRGBAF32(in_name.data(), &in_pModifier->Color, in_speed);

            ImGui::PopID();
        };

        createColorControls("Top Left", &in_pModifier->TL);
        createColorControls("Bottom Left", &in_pModifier->BL);
        createColorControls("Top Right", &in_pModifier->TR);
        createColorControls("Bottom Right", &in_pModifier->BR);

        ImGui::TreePop();
    }
}

static void DrawModifiers(CsdVertexModifierGroup* in_pModifier)
{
    DrawPositionModifierNode("Position", in_pModifier);
    DrawUVModifierNode("UV", in_pModifier);
    DrawColorModifierNode("Color", in_pModifier);
}

static bool DrawVertexModifierGroupNode(std::string_view in_name, CsdVertexModifierGroup* in_pModifier, bool in_isSlice = false)
{
    bool isDeleting{};

    if (ImGui::TreeNode(in_name.data()))
    {
        if (in_isSlice)
        {
            if (ImGui::TreeNode("Flags"))
            {
                if (CreateFlagsTable({ 0, 256.0f }))
                {
                    CreateFlagsRow("Flags", &in_pModifier->Flags);
                    ImGui::EndTable();
                }

                ImGui::TreePop();
            }
        }

        DrawModifiers(in_pModifier);

        if (!in_isSlice)
        {
            if (ImGui::TreeNode("Slices"))
            {
                if (ImGui::Button("Add"))
                    in_pModifier->Slices.push_back(CsdVertexModifierGroup());

                if (!in_pModifier->Slices.empty())
                {
                    ImGui::Separator();

                    std::optional<size_t> deleteIndex;

                    for (size_t i = 0; i < in_pModifier->Slices.size(); i++)
                    {
                        ImGui::PushID(i);

                        if (DrawVertexModifierGroupNode(std::format("{}", i).c_str(), &in_pModifier->Slices[i], true))
                            deleteIndex = i;

                        ImGui::PopID();
                    }

                    if (deleteIndex)
                        in_pModifier->Slices.erase(in_pModifier->Slices.begin() + deleteIndex.value());
                }

                ImGui::TreePop();
            }
        }

        ImGui::Separator();

        if (in_isSlice)
        {
            if (ImGui::Button("Delete"))
                isDeleting = true;
        }

        ImGui::TreePop();
    }

    return isDeleting;
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
        ImGui::SetNextWindowSize({ 733.0f, 905.0f });
        hasSetWindowSize = true;
    }

    if (ImGui::Begin("CSD Modifier Manager", &isVisible))
    {
        if (ImGui::CollapsingHeader("Configuration", ImGuiTreeNodeFlags_DefaultOpen))
        {
            static std::array<const char*, 2> alignmentModes = { "Edge", "Center" };
            ImGui::Combo("UI Alignment Mode", reinterpret_cast<int*>(&Config::UIAlignmentMode.Value), alignmentModes.data(), alignmentModes.size());

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
            static size_t totalModifierSize{};
            static size_t activeModifierSize{};
            static size_t activeModifierCount{};

            if (ImGui::BeginTable("##Diagnostics", 2))
            {
                size_t projectsLength{};

                for (const auto& [rProject, rSize] : g_Projects)
                    projectsLength += rSize + sizeof(Chao::CSD::Project);

                IMGUI_GENERIC_ROW("Total Modifiers", "%zu (%.2f KiB)", g_Modifiers.size(), static_cast<double>(totalModifierSize / 1024.0));
                IMGUI_GENERIC_ROW("Active Modifiers", "%zu (%.2f KiB)", activeModifierCount, static_cast<double>(activeModifierSize / 1024.0));
                IMGUI_GENERIC_ROW("Projects", "%zu (%.2f KiB)", g_Projects.size(), static_cast<double>(projectsLength / 1024.0));
                IMGUI_GENERIC_ROW("Nodes", "%zu", g_Paths.size());
                ImGui::EndTable();
            }

            activeModifierCount = 0;

            enum
            {
                Filter_All,
                Filter_Active,
                Filter_Inactive,
            };

            static int filter = Filter_Active;

            ImGui::Separator();

            static char searchBuffer[256]{};
            const auto hasSearchQuery = searchBuffer[0] != '\0';
            ImGui::InputText("Search", reinterpret_cast<char*>(&searchBuffer), sizeof(searchBuffer));

            static std::array<const char*, 3> filters = { "All", "Active", "Inactive" };
            ImGui::Combo("Filter", &filter, filters.data(), filters.size());

            const auto expandAll = ImGui::Button("Expand All", { ImGui::GetContentRegionAvail().x * 0.5f, 0.0f });
            ImGui::SameLine();
            const auto collapseAll = ImGui::Button("Collapse All", { ImGui::GetContentRegionAvail().x, 0.0f });
            ImGui::Separator();

            const auto footerHeight = ImGui::GetFrameHeight() + ImGui::GetStyle().ItemSpacing.y + 4.0f;

            if (ImGui::BeginChild("##Modifiers", { 0, -footerHeight }))
            {
                totalModifierSize = 0;
                activeModifierSize = 0;

                for (auto& [rModifierHash, rModifier] : g_Modifiers)
                {
                    uintptr_t addr{};

                    totalModifierSize += rModifier.GetSize();

                    // Find first reference to this modifier.
                    for (const auto& [rPath, rPathHash] : g_Paths)
                    {
                        if (rModifierHash == rPathHash)
                        {
                            addr = uintptr_t(rPath);
                            activeModifierSize += rModifier.GetSize();
                            activeModifierCount++;
                            break;
                        }
                    }

                    if (filter != Filter_All && filter != (addr ? Filter_Active : Filter_Inactive))
                        continue;

                    if (hasSearchQuery && rModifier.Path.find(reinterpret_cast<const char*>(&searchBuffer)) == std::string::npos)
                        continue;

                    ImGui::PushID(addr);

                    if (expandAll)
                        ImGui::SetNextItemOpen(true);
                    else if (collapseAll)
                        ImGui::SetNextItemOpen(false);

                    if (!addr)
                    {
                        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.62f, 0.62f, 0.62f, 1.0f));
                    }

                    const auto isHeaderOpen = ImGui::CollapsingHeader(rModifier.Path.c_str());

                    if (!addr)
                        ImGui::PopStyleColor(3);

                    if (isHeaderOpen)
                    {
                        if (ImGui::TreeNodeEx("Flags", ImGuiTreeNodeFlags_DefaultOpen))
                        {
                            if (CreateFlagsTable({ 0, 398.0f }))
                            {
                                CreateFlagsRow("Flags", &rModifier.Flags);
                                CreateFlagsRow("Ultrawide Mask", &rModifier.Ultrawide.Mask);
                                CreateFlagsRow("Ultrawide Edge Mask", &rModifier.Ultrawide.Edge.Mask);
                                CreateFlagsRow("Ultrawide Center Mask", &rModifier.Ultrawide.Center.Mask);
                                CreateFlagsRow("Narrow Mask", &rModifier.Narrow.Mask);
                                CreateFlagsRow("Narrow Edge Mask", &rModifier.Narrow.Edge.Mask);
                                CreateFlagsRow("Narrow Center Mask", &rModifier.Narrow.Center.Mask);
                                ImGui::EndTable();
                            }

                            if (ImGui::Button("Enable All", { ImGui::GetContentRegionAvail().x * 0.5f, 0 }))
                                rModifier.Flags |= ~0ULL;

                            ImGui::SameLine();

                            if (ImGui::Button("Disable All", { ImGui::GetContentRegionAvail().x, 0 }))
                                rModifier.Flags &= ~rModifier.Flags;

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

                                createRawFlagsControls("Flags", rModifier.Flags);
                                createRawFlagsControls("Ultrawide Mask", rModifier.Ultrawide.Mask);
                                createRawFlagsControls("Ultrawide Edge Mask", rModifier.Ultrawide.Edge.Mask);
                                createRawFlagsControls("Ultrawide Center Mask", rModifier.Ultrawide.Center.Mask);
                                createRawFlagsControls("Narrow Mask", rModifier.Narrow.Mask);
                                createRawFlagsControls("Narrow Edge Mask", rModifier.Narrow.Edge.Mask);
                                createRawFlagsControls("Narrow Center Mask", rModifier.Narrow.Center.Mask);

                                ImGui::TreePop();
                            }

                            ImGui::TreePop();
                        }

                        if (ImGui::TreeNode("Vertices"))
                        {
                            const auto drawAlignmentModifierNode = [&](std::string_view in_name, CsdAlignmentModifier* in_pModifier)
                            {
                                if (ImGui::TreeNode(in_name.data()))
                                {
                                    DrawVertexModifierGroupNode("Base", &in_pModifier->Base);
                                    DrawVertexModifierGroupNode("Repeat", &in_pModifier->Repeat);
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

                            drawAlignmentModifierGroupNode("Ultrawide", &rModifier.Ultrawide);
                            drawAlignmentModifierGroupNode("Narrow", &rModifier.Narrow);

                            ImGui::TreePop();
                        }

                        if (ImGui::TreeNode("Corners"))
                        {
                            if (ImGui::BeginTable("##Corners", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
                            {
                                for (int i = 0; i < g_csdCorners.size(); i++)
                                {
                                    ImGui::TableNextColumn();
                                    ImGui::AlignTextToFramePadding();
                                    ImGui::Text("%f", g_csdCorners[i]);
                                }

                                ImGui::EndTable();
                            }

                            ImGui::DragFloat("Corner Max", &rModifier.CornerMax, 0.01f);
                            ImGui::SliderInt("Corner Index", &rModifier.CornerIndex, 0, 7);

                            ImGui::TreePop();
                        }

                        if (ImGui::TreeNode("References"))
                        {
                            if (ImGui::BeginTable("##References", 2))
                            {
                                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);

                                for (const auto& [rPath, rPathHash] : g_Paths)
                                {
                                    if (rModifierHash != rPathHash)
                                        continue;

                                    ImGui::PushID(rPath);

                                    ImGui::TableNextColumn();
                                    ImGui::AlignTextToFramePadding();

                                    const auto text = std::format("0x{:X}", uintptr_t(rPath));

                                    ImGui::Text("- %s", text.c_str());
                                    ImGui::TableNextColumn();

                                    if (ImGui::Button("Copy"))
                                        ImGui::SetClipboardText(text.c_str());

                                    ImGui::PopID();
                                }

                                ImGui::EndTable();
                            }

                            ImGui::TreePop();
                        }

                        ImGui::Separator();

                        if (ImGui::Button("Copy Definition"))
                            ImGui::SetClipboardText(rModifier.ToString().c_str());

                        ImGui::SameLine();

                        if (ImGui::Button("Delete"))
                        {
                            ImGui::OpenPopup("Delete Modifier?");
                            ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
                        }

                        if (ImGui::BeginPopupModal("Delete Modifier?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
                        {
                            ImGui::Text("Are you sure you want to delete this modifier?\nThis action is irreversible until next reload.");
                            ImGui::Separator();

                            if (ImGui::Button("Yes"))
                            {
                                // Delete modifier from all references.
                                for (const auto& [rPath, rPathHash] : g_Paths)
                                {
                                    if (rModifierHash != rPathHash)
                                        continue;

                                    deletions.push_back(rPath);
                                }

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

    ImGui::End();
    ImGui::PopFont();

    pFont->Scale = defaultFontScale;

    std::lock_guard lock(g_Mutex);

    for (const auto& rDeleted : deletions)
        g_Paths.erase(rDeleted);

    deletions.clear();
}
