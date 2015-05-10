paani

CIS 660 Authoring Tool Project
debanshu singh & sanchit garg

Based on - Miles Macklin, Matthias Müller: Position Based Fluids, ACM Transactions on Graphics (SIGGRAPH 2013)

============
Requirements
============
Mac OS X (10.10)
Houdini 14.0.258
zlib 1.2.8
OpenGL 3.3+
intel tbb 4.3

===================
Thirdparty Libaries
===================
Open Asset Import Library (libassimp)
glm 0.9.3
glfw 3
glew 1.11

=====
Usage
=====
01. Open paani.xcodeproj
02. Choose Product->Build to build Houdini14 plugin named "SOP_Paani.dylib" at project root
For Developers - Note that Build has two steps. 
- First, it compiles pbf library. 
- Second, it compiles the pbf library with the HDK plugin "SOP_Paani.dylib". It needs Houdini's hcustom tool, Mac OS X's clang & libstdc++ to link correctly.
03. Once built, make sure to add/copy SOP_Paani.dylib to HOUDINI_DSO_PATH
04. You should now be able to use the Paani SOP in Houdini 14.

For some examples of use in Houdini, look at paani_examples.pdf

==================
Thirdparty Sources
==================
01. http://ogldev.atspace.co.uk/www/tutorial22/tutorial22.html​­ o​bj importer code in Mesh class
02. http://www.tomdalling.com/blog/modern­opengl/01­getting­started­in­xcode­and ­visual­cpp/​­ GLFW window setup code
03. http://insanecoding.blogspot.com/2011/11/how­to­read­in­file­in­c.html
04. http://lists.apple.com/archives/mac­opengl/2012/Jul/msg00038.html
05. Utility Core ­ print functions and math constants (Yining Karl Li) 06. www.sidefx.com/docs/hdk14.0/_h_d_k__intro__compiling.html
07. http://www.deborahrfowler.com/C++Resources/HDK/HDK12_Intro.pdf
08. http://www.orbolt.com​­ houdini render backdrop geometry & sky hdr