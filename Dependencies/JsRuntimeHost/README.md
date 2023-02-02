[![Build Status](https://dev.azure.com/babylonjs/ContinousIntegration/_apis/build/status/JsRuntimeHost%20CI?branchName=main)](https://dev.azure.com/babylonjs/ContinousIntegration/_build/latest?definitionId=22&branchName=main)

# JavaScript Runtime Host
The JsRuntimeHost is a library that provides cross-platform C++ JavaScript hosting for
any JavaScript engines with Node-API support such as Chakra, V8, or JavaScriptCore. The
Node-API contract from Node.js allows consumers of this library to interact with the
JavaScript engine with a consistent interface. This library also provides some optional
polyfills that consumers can include if required.

## Contributing

Please read [CONTRIBUTING.md](./CONTRIBUTING.md) for details on our code of conduct, and 
the process for submitting pull requests.

## Reporting Security Issues

Security issues and bugs should be reported privately, via email, to the Microsoft 
Security Response Center (MSRC) at [secure@microsoft.com](mailto:secure@microsoft.com). 
You should receive a response within 24 hours. If for some reason you do not, please 
follow up via email to ensure we received your original message. Further information, 
including the [MSRC PGP](https://technet.microsoft.com/en-us/security/dn606155) key, can 
be found in the [Security TechCenter](https://technet.microsoft.com/en-us/security/default).
