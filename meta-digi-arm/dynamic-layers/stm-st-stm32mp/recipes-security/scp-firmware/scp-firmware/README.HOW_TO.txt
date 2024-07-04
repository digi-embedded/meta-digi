Sharing scp firmware
1. Prepare scp firmware source
2. Manage scp firmware source code with GIT
3. Configure scp firmware source code
4. Test scp firmware source code

--------------------------------------
1. Prepare scp-firmware source
--------------------------------------
If not already done, extract the sources from Developer Package tarball, for example:
    $ tar xf en.SOURCES-stm32mp1-*.tar.xz

In the scp firmware source directory (sources/*/##BP##-##PR##),
you have one external dt tarball:
   - ##BP##-##PR##.tar.xz
   - 00*.patch

If you would like to have a git management for the source code move to
to section 2 [Management of external dt source code with GIT].

Otherwise, to manage scp firmware source code without git, you must extract the
tarball now and apply the patch:

    $> tar xf ##BP##-##PR##.tar.xz
    $> cd ##BP##
    $> for p in `ls -1 ../*.patch`; do patch -p1 < $p; done

You can now move to section 3 [Configure scp firmware source code].

-------------------------------------
2. Manage external dt source code with GIT
-------------------------------------
If you like to have a better management of change made on external dt source, you
have following solutions to use git.

2.1 Create Git from tarball
---------------------------
    $ cd <directory to scp firmware source code>
    $ test -d .git || git init . && git add . && git commit -m "new scp-firwmare" && git gc
    $ git checkout -b WORKING
  NB: this is the fastest way to get your scp firmware source code ready for development

-------------------------------
3. Configure scp firmware source code
-------------------------------
To enable use of scp firmware source code for other component, you must set the
SCPFW_DIR variable to your shell environement:

    $> export SCPFW_DIR=$PWD/##BP##

---------------------------
4. Test scp firmware source code
---------------------------
Nothing to do, scp-firmware is directly used by other component.

    #> echo "*** Nothing to test ***"
