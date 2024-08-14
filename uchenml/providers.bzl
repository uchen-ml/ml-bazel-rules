""" Providers for the dataset module. """

DatasetInfo = provider(
    doc = "Information about a dataset.",
    fields = {
        "manifests": "The manifest files for the dataset.",
        "directory": "The directory containing the dataset.",
    },
)
