""" Utilities for the uchenml rules. """

def build_outputs(name, files, max_files_per_dir, ext="sample"):
    """
    Generate output file names for a rule that produces multiple files.

    Args:
        name: The rule name that will be used as a directory name.
        files: The number of output files.
        max_files_per_dir: The maximum number of files per directory.
        ext: The file extension.
    Returns:
        A list of output file names.
    """
    name_gen = lambda i: "%s/%d.%s" % (name, i + 1, ext)
    if max_files_per_dir < files:
        dirs = (files + max_files_per_dir - 1) // max_files_per_dir
        name_gen = lambda i: "%s/%d/%d.%s" % (
            name,
            (i % dirs) + 1,
            (i // dirs) + 1,
            ext
        )
    return sorted([name_gen(i) for i in range(files)])
