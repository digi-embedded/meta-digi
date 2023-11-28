# "master" branch was renamed to "main", which causes do_fetch() to fail.
# Reflect this in SRC_URI.
SRC_URI:remove = "git://github.com/posborne/cmsis-svd.git;protocol=https;branch=master"
SRC_URI:append = " git://github.com/posborne/cmsis-svd.git;protocol=https;branch=main"
