Import("env")
import os

def run_native(source, target, env):
    bin_path = os.path.join(env.subst("$BUILD_DIR"), "program")
    os.system(bin_path)

env.AddPostAction("buildprog", run_native)
