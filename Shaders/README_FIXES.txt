Corrections appliquees:
- Main.cpp utilise StandardVertex pour la pipeline, comme Model.
- UniformBufferObject: Model, View, Projection, InverseView.
- shader.vert: UBO et attributs coherents avec StandardVertex.
- Vert.spv/Frag.spv: versions correspondantes presentes dans ShadersBackup/4_*.
- ShaderCompile.bat: compilation independante du dossier courant via %VULKAN_SDK%.
