# TI_Stable

Topological interlocking tolerance analysis.
Only testing in **OSX 1O.13**.
Please send email to qiqiustc@gmail.com if you have any problem.

## Dependency
1. libigl, please refer to https://github.com/libigl/libigl

    please put libigl in the **same folder** as this project.

2. Eigen3, please refer to http://eigen.tuxfamily.org

    actually, libigl/nanogui has eigen library

3. Mosek, please refer to https://www.mosek.com

    a. please put the mosek folder in **/usr/local**

    b. compile the mosek library http://docs.mosek.com/8.1/install/installation.html

    c. compile the mosek c++ fusion http://docs.mosek.com/8.1/cxxfusion/install-interface.html

    d. copy all **.dylib** from **/usr/local/mosek/8/tools/platform/osx64x86/bin** into **\usr\local\lib**

    e. put mosek license in right position, please refer to http://docs.mosek.com/7.0/toolsinstall/Obtaining_and_installing_a_license.html

    f. please note that there is a bug in **igl::mosek::mosek_linprog**.

## Compile

1. Download the Clion IDE from https://www.jetbrains.com/clion/
2. Use **open project** in Clion IDE
3. Build project
4. Run