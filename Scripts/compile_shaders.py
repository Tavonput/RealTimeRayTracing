import os
import subprocess

from glob import glob

class ShaderCompiler:
    def __init__(self) -> None:
        pass

    def compile(self) -> None:
        vulkan_version = self._find_vulkan_version()
        self._check_shader_directory()
        
        executable = f"../Vendor/VulkanSDK/{vulkan_version}/Bin/glslc.exe"
        source_dir = "../RayTrace/Src/Shaders"
        bin_dir    = "../Bin/Shaders"
        extensions = ["*.vert", "*.frag", "*.rgen", "*.rchit", "*.rmiss", "*.rahit"]

        # Find all shader sources to compile
        source_paths = []
        for ext in extensions:
            source_paths.extend(glob(f"{source_dir}/{ext}"))

        # Compile
        for source_path in source_paths:
            name = os.path.basename(source_path)
            name, ext = os.path.splitext(name)
            bin_path = f"{bin_dir}/{name}_{ext[1:]}.spv"

            print(f"Compiling [{source_path}] ---> [{bin_path}]")
            subprocess.run([executable, source_path, "-o", bin_path, "--target-env=vulkan1.3"])

        print("Compilation finished")

    def _find_vulkan_version(self) -> str:
        parentDirectory = "../Vendor/VulkanSDK"

        # There should only be one subdirectory
        subDirectories = [d for d in os.listdir(parentDirectory)]
        return subDirectories[0]

    def _check_shader_directory(self) -> None:
        bin_dir = "../Bin"
        shader_dir = "../Bin/Shaders"

        # Check for binaries directory
        if not os.path.exists(bin_dir):
            os.makedirs(bin_dir)
            print(f"Created directory {bin_dir}")

        # Check for shaders directory
        if not os.path.exists(shader_dir):
            os.makedirs(shader_dir)
            print(f"Created directory {shader_dir}")


if __name__ == "__main__":
    compiler = ShaderCompiler()
    compiler.compile()