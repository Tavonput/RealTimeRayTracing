import os
import subprocess

def find_vulkan_version() -> str:
    parentDirectory = "../Vendor/VulkanSDK"

    # There should only be one subdirectory
    subDirectories = [d for d in os.listdir(parentDirectory)]
    return subDirectories[0]

def compile_shaders(vulkan_version):
    executable = f"../Vendor/VulkanSDK/{vulkan_version}/Bin/glslc.exe"

    # Vertex shader
    print("Compiling vertex shader")
    vert_src = "../RayTrace/Src/Shaders/shader.vert"
    vert_bin = "../Bin/Shaders/shader_vert.spv"
    subprocess.run([executable, vert_src, "-o", vert_bin])

    # Fragment shader
    print("Compiling fragment shader")
    frag_src = "../RayTrace/Src/Shaders/shader.frag"
    frag_bin = "../Bin/Shaders/shader_frag.spv"
    subprocess.run([executable, frag_src, "-o", frag_bin])

    print("Compilation finished")

if __name__ == "__main__":
    vulkan_version = find_vulkan_version()
    compile_shaders(vulkan_version)
