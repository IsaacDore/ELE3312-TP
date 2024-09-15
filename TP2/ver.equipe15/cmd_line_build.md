[keil faq](https://www.keil.com/support/docs/868.htm)

To build your project from the command line...

For example, if you have a project file (PROJECT.uvproj) with a target you want to build (TARGET NUMBER 1), you may use the following command line to build the project and exit when the build process is complete.

`UV4 -b PROJECT.UVproj -t"TARGET NUMBER 1"`

If you want to redirect the output to a file, you may use the -o option. For example:

`UV4 -b PROJECT.UVproj -t"TARGET NUMBER 1" -o"OUTPUTFILE.TXT"`

donc dans ce cas:
    à partir de \[...\]\\test1\\MDK-ARM\\
    `UV4 -b test1.uvprojx -t test1 -o"./log/build-ouput.txt"`

[liste des commandes](https://www.keil.com/support/man/docs/uv4cl/uv4cl_commandline.htm)

upload
`UV4 -f test1.uvprojx -o"./log/upload-output.txt"` 