# Copyright (C) 2025, Digi International Inc.

# Overwrite search_path() function in original 'sign-stm2mp.bbclass'
def search_path(file_search, d, err_not_found=False):
    """
    Search for <file_search> in BBPATH and return its absolute path.
    If the file is not found:
        - Returns the original file string if err_not_found is False.
        - Otherwise, it triggers a fatal error.
    """
    search_path = d.getVar("BBPATH").split(":")
    for p in search_path:
        file_path = os.path.join(p, file_search)
        if os.path.isfile(file_path):
            return file_path

    # If file is not found
    bbpaths = d.getVar('BBPATH').replace(':','\n\t')
    bb.debug(1, '\n[sign-stm32mp-digi] Not able to find "%s" path from current BBPATH' % (file_search))

    if not err_not_found:
        return file_search # Return original file string instead of failing

    bb.fatal('\n[sign-stm32mp-digi] Not able to find "%s" path from current BBPATH var:\n\t%s.' % (file_search, bbpaths))
