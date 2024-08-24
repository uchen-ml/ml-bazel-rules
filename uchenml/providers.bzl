""" Providers for the dataset module. """

DatasetInfo = provider(
    doc = "Information about a dataset.",
    fields = {
        "data": "Directory containing the dataset.",
        "max_samples": "Number of samples in the dataset. Note that we may not know the real number until the build finishes.",
    },
)
