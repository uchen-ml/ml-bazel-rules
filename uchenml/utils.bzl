""" Utility functions """

def samples_dir_count(samples):
    """
    Simple way to decide on how many directories we need for samples count
    """
    if samples < 250:
        return 1
    return samples // 250