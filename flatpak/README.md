Build
```sh
flatpak-builder --force-clean --user --install-deps-from=flathub --repo=repo --install builddir io.github.ozordi.libertyrecomp.json
```

Bundle
```sh
flatpak build-bundle repo io.github.ozordi.libertyrecomp.flatpak io.github.ozordi.libertyrecomp --runtime-repo=https://flathub.org/repo/flathub.flatpakrepo
```
