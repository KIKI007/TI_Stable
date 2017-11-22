# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.glfw.Debug:
PostBuild.glfw_objects.Debug: /Users/ziqwang/Documents/Xcode/TI/build/glfw/src/Debug/libglfw3.a
/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/Debug/libglfw3.a:\
	/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/GLFW.build/Debug/glfw_objects.build/Objects-normal/libglfw_objects.a
	/bin/rm -f /Users/ziqwang/Documents/Xcode/TI/build/glfw/src/Debug/libglfw3.a


PostBuild.glfw_objects.Debug:
PostBuild.glfw.Release:
PostBuild.glfw_objects.Release: /Users/ziqwang/Documents/Xcode/TI/build/glfw/src/Release/libglfw3.a
/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/Release/libglfw3.a:\
	/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/GLFW.build/Release/glfw_objects.build/Objects-normal/libglfw_objects.a
	/bin/rm -f /Users/ziqwang/Documents/Xcode/TI/build/glfw/src/Release/libglfw3.a


PostBuild.glfw_objects.Release:
PostBuild.glfw.MinSizeRel:
PostBuild.glfw_objects.MinSizeRel: /Users/ziqwang/Documents/Xcode/TI/build/glfw/src/MinSizeRel/libglfw3.a
/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/MinSizeRel/libglfw3.a:\
	/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/GLFW.build/MinSizeRel/glfw_objects.build/Objects-normal/libglfw_objects.a
	/bin/rm -f /Users/ziqwang/Documents/Xcode/TI/build/glfw/src/MinSizeRel/libglfw3.a


PostBuild.glfw_objects.MinSizeRel:
PostBuild.glfw.RelWithDebInfo:
PostBuild.glfw_objects.RelWithDebInfo: /Users/ziqwang/Documents/Xcode/TI/build/glfw/src/RelWithDebInfo/libglfw3.a
/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/RelWithDebInfo/libglfw3.a:\
	/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/GLFW.build/RelWithDebInfo/glfw_objects.build/Objects-normal/libglfw_objects.a
	/bin/rm -f /Users/ziqwang/Documents/Xcode/TI/build/glfw/src/RelWithDebInfo/libglfw3.a


PostBuild.glfw_objects.RelWithDebInfo:


# For each target create a dummy ruleso the target does not have to exist
/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/GLFW.build/Debug/glfw_objects.build/Objects-normal/libglfw_objects.a:
/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/GLFW.build/MinSizeRel/glfw_objects.build/Objects-normal/libglfw_objects.a:
/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/GLFW.build/RelWithDebInfo/glfw_objects.build/Objects-normal/libglfw_objects.a:
/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/GLFW.build/Release/glfw_objects.build/Objects-normal/libglfw_objects.a:
