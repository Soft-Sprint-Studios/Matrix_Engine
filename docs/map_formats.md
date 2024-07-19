# Map formats

# Compiling levels:
Pathos's compilers are a modified version of VLuzacn's HL tools, which were based on
Zoner's HL tools. These output a BSP with the modified Pathos BSP V1 format, which
enables larger map sizes and more resources to be used. For this purpose, I have
a number of compile scripts, located under the pathos/sources folder. The best way 
to compile levels is to export the .map files into the pathos/sources/compile folder, 
and use one of the compile scripts to compile the levels.
 
# HL1BSP:
Pathos still supports HL1BSP due to legacy support reasons, but I generally don't
believe you have any reason to stick to this format.

> [!NOTE]
> It is generally best advised to not go too far beyond the +-4096 coordinates limit, as
> due to precision issues with the BSP format, small texture scales can cause issues that
> can make your compiles fail.