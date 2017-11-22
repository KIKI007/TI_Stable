# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.example_bin.Debug:
PostBuild.igl_core.Debug: /Users/ziqwang/Documents/Xcode/TI/build/Debug/example_bin
PostBuild.igl_viewer.Debug: /Users/ziqwang/Documents/Xcode/TI/build/Debug/example_bin
PostBuild.igl_core.Debug: /Users/ziqwang/Documents/Xcode/TI/build/Debug/example_bin
PostBuild.igl_common.Debug: /Users/ziqwang/Documents/Xcode/TI/build/Debug/example_bin
PostBuild.glfw.Debug: /Users/ziqwang/Documents/Xcode/TI/build/Debug/example_bin
PostBuild.glew.Debug: /Users/ziqwang/Documents/Xcode/TI/build/Debug/example_bin
/Users/ziqwang/Documents/Xcode/TI/build/Debug/example_bin:\
	/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/Debug/libglfw3.a\
	/Users/ziqwang/Documents/Xcode/TI/build/Debug/libglew.a
	/bin/rm -f /Users/ziqwang/Documents/Xcode/TI/build/Debug/example_bin


PostBuild.glew.Debug:
/Users/ziqwang/Documents/Xcode/TI/build/Debug/libglew.a:
	/bin/rm -f /Users/ziqwang/Documents/Xcode/TI/build/Debug/libglew.a


PostBuild.glfw.Debug:
PostBuild.glfw_objects.Debug: /Users/ziqwang/Documents/Xcode/TI/build/glfw/src/Debug/libglfw3.a
/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/Debug/libglfw3.a:\
	/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/example.build/Debug/glfw_objects.build/Objects-normal/libglfw_objects.a
	/bin/rm -f /Users/ziqwang/Documents/Xcode/TI/build/glfw/src/Debug/libglfw3.a


PostBuild.glfw_objects.Debug:
PostBuild.example_bin.Release:
PostBuild.igl_core.Release: /Users/ziqwang/Documents/Xcode/TI/build/Release/example_bin
PostBuild.igl_viewer.Release: /Users/ziqwang/Documents/Xcode/TI/build/Release/example_bin
PostBuild.igl_core.Release: /Users/ziqwang/Documents/Xcode/TI/build/Release/example_bin
PostBuild.igl_common.Release: /Users/ziqwang/Documents/Xcode/TI/build/Release/example_bin
PostBuild.glfw.Release: /Users/ziqwang/Documents/Xcode/TI/build/Release/example_bin
PostBuild.glew.Release: /Users/ziqwang/Documents/Xcode/TI/build/Release/example_bin
/Users/ziqwang/Documents/Xcode/TI/build/Release/example_bin:\
	/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/Release/libglfw3.a\
	/Users/ziqwang/Documents/Xcode/TI/build/Release/libglew.a
	/bin/rm -f /Users/ziqwang/Documents/Xcode/TI/build/Release/example_bin


PostBuild.glew.Release:
/Users/ziqwang/Documents/Xcode/TI/build/Release/libglew.a:
	/bin/rm -f /Users/ziqwang/Documents/Xcode/TI/build/Release/libglew.a


PostBuild.glfw.Release:
PostBuild.glfw_objects.Release: /Users/ziqwang/Documents/Xcode/TI/build/glfw/src/Release/libglfw3.a
/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/Release/libglfw3.a:\
	/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/example.build/Release/glfw_objects.build/Objects-normal/libglfw_objects.a
	/bin/rm -f /Users/ziqwang/Documents/Xcode/TI/build/glfw/src/Release/libglfw3.a


PostBuild.glfw_objects.Release:
PostBuild.example_bin.MinSizeRel:
PostBuild.igl_core.MinSizeRel: /Users/ziqwang/Documents/Xcode/TI/build/MinSizeRel/example_bin
PostBuild.igl_viewer.MinSizeRel: /Users/ziqwang/Documents/Xcode/TI/build/MinSizeRel/example_bin
PostBuild.igl_core.MinSizeRel: /Users/ziqwang/Documents/Xcode/TI/build/MinSizeRel/example_bin
PostBuild.igl_common.MinSizeRel: /Users/ziqwang/Documents/Xcode/TI/build/MinSizeRel/example_bin
PostBuild.glfw.MinSizeRel: /Users/ziqwang/Documents/Xcode/TI/build/MinSizeRel/example_bin
PostBuild.glew.MinSizeRel: /Users/ziqwang/Documents/Xcode/TI/build/MinSizeRel/example_bin
/Users/ziqwang/Documents/Xcode/TI/build/MinSizeRel/example_bin:\
	/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/MinSizeRel/libglfw3.a\
	/Users/ziqwang/Documents/Xcode/TI/build/MinSizeRel/libglew.a
	/bin/rm -f /Users/ziqwang/Documents/Xcode/TI/build/MinSizeRel/example_bin


PostBuild.glew.MinSizeRel:
/Users/ziqwang/Documents/Xcode/TI/build/MinSizeRel/libglew.a:
	/bin/rm -f /Users/ziqwang/Documents/Xcode/TI/build/MinSizeRel/libglew.a


PostBuild.glfw.MinSizeRel:
PostBuild.glfw_objects.MinSizeRel: /Users/ziqwang/Documents/Xcode/TI/build/glfw/src/MinSizeRel/libglfw3.a
/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/MinSizeRel/libglfw3.a:\
	/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/example.build/MinSizeRel/glfw_objects.build/Objects-normal/libglfw_objects.a
	/bin/rm -f /Users/ziqwang/Documents/Xcode/TI/build/glfw/src/MinSizeRel/libglfw3.a


PostBuild.glfw_objects.MinSizeRel:
PostBuild.example_bin.RelWithDebInfo:
PostBuild.igl_core.RelWithDebInfo: /Users/ziqwang/Documents/Xcode/TI/build/RelWithDebInfo/example_bin
PostBuild.igl_viewer.RelWithDebInfo: /Users/ziqwang/Documents/Xcode/TI/build/RelWithDebInfo/example_bin
PostBuild.igl_core.RelWithDebInfo: /Users/ziqwang/Documents/Xcode/TI/build/RelWithDebInfo/example_bin
PostBuild.igl_common.RelWithDebInfo: /Users/ziqwang/Documents/Xcode/TI/build/RelWithDebInfo/example_bin
PostBuild.glfw.RelWithDebInfo: /Users/ziqwang/Documents/Xcode/TI/build/RelWithDebInfo/example_bin
PostBuild.glew.RelWithDebInfo: /Users/ziqwang/Documents/Xcode/TI/build/RelWithDebInfo/example_bin
/Users/ziqwang/Documents/Xcode/TI/build/RelWithDebInfo/example_bin:\
	/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/RelWithDebInfo/libglfw3.a\
	/Users/ziqwang/Documents/Xcode/TI/build/RelWithDebInfo/libglew.a
	/bin/rm -f /Users/ziqwang/Documents/Xcode/TI/build/RelWithDebInfo/example_bin


PostBuild.glew.RelWithDebInfo:
/Users/ziqwang/Documents/Xcode/TI/build/RelWithDebInfo/libglew.a:
	/bin/rm -f /Users/ziqwang/Documents/Xcode/TI/build/RelWithDebInfo/libglew.a


PostBuild.glfw.RelWithDebInfo:
PostBuild.glfw_objects.RelWithDebInfo: /Users/ziqwang/Documents/Xcode/TI/build/glfw/src/RelWithDebInfo/libglfw3.a
/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/RelWithDebInfo/libglfw3.a:\
	/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/example.build/RelWithDebInfo/glfw_objects.build/Objects-normal/libglfw_objects.a
	/bin/rm -f /Users/ziqwang/Documents/Xcode/TI/build/glfw/src/RelWithDebInfo/libglfw3.a


PostBuild.glfw_objects.RelWithDebInfo:


# For each target create a dummy ruleso the target does not have to exist
/Users/ziqwang/Documents/Xcode/TI/build/Debug/libglew.a:
/Users/ziqwang/Documents/Xcode/TI/build/MinSizeRel/libglew.a:
/Users/ziqwang/Documents/Xcode/TI/build/RelWithDebInfo/libglew.a:
/Users/ziqwang/Documents/Xcode/TI/build/Release/libglew.a:
/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/Debug/libglfw3.a:
/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/MinSizeRel/libglfw3.a:
/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/RelWithDebInfo/libglfw3.a:
/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/Release/libglfw3.a:
/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/example.build/Debug/glfw_objects.build/Objects-normal/libglfw_objects.a:
/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/example.build/MinSizeRel/glfw_objects.build/Objects-normal/libglfw_objects.a:
/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/example.build/RelWithDebInfo/glfw_objects.build/Objects-normal/libglfw_objects.a:
/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/example.build/Release/glfw_objects.build/Objects-normal/libglfw_objects.a:
