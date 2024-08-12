""" Providers for the dataset module. """

DatasetInfo = provider(
    doc = "Information about a dataset.",
    fields = {
        "manifest": "The manifest file for the dataset.",
        "directory": "The directory containing the dataset.",
    },
)
