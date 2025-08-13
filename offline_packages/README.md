# Offline packages

This directory can hold pre-downloaded `.deb` archives for following the
[environment setup guide](../docs/setup.md) without network access.

Use `apt-get download <package>` or `apt-get install --download-only <package>`
to fetch the required packages on a machine with internet access.
Copy the resulting `.deb` files here and install them manually with
`dpkg -i` as described in the guide.
