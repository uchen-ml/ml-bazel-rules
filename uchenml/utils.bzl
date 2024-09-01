""" Utilities for the uchenml rules. """

def build_outputs(name, dirs, sample_count):
    name_gen = lambda i: "%s/%d.sample" % (name, i + 1)
    if dirs > 1:
        name_gen = lambda i: "%s/%d/%d.sample" % (
            name,
            (i % dirs) + 1,
            (i // dirs) + 1,
        )
    return sorted([name_gen(i) for i in range(sample_count)])
