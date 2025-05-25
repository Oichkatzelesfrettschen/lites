# Offline packages

This directory can hold pre-downloaded `.deb` archives for running
`setup.sh` without network access.

Use `apt-get download <package>` or `apt-get install --download-only <package>`
to fetch the required packages on a machine with internet access.
Copy the resulting `.deb` files here.  When `setup.sh --offline` is
invoked, all archives in this directory are installed with
`dpkg -i`.
