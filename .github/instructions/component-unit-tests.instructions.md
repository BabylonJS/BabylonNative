---
applyTo: ".github/actions/**,.github/workflows/**"
---

# Component platform unit tests

Put component-specific build and unit-test steps in composite actions:

- `.github/actions/urllib/{win32,linux,macos}/action.yml`
- `.github/actions/jsruntimehost/{win32,linux,macos,android,ios}/action.yml`

Follow `.github/actions/<component>/<platform>/action.yml` for additional
components or supported platforms. Do not create nested workflows under
`.github/workflows`: GitHub does not discover or call workflows in subfolders.
Do not add empty platform actions for components without a test harness.

Keep workflow triggers, runners, shared toolchain setup and root CMake
configuration in the top-level `.github/workflows/*.yml` files. After checkout,
invoke the local component action with a step-level `uses` and an `if` using
that component's source-change output. Do not inline its build/test commands
back into the platform workflow. Desktop actions consume the configured build
directory; mobile actions own their separate test-harness configuration.

PR checks gate the components independently: JsRuntimeHost changes enable
JsRuntimeHost tests, and UrlLib changes enable UrlLib tests. Changes to a
component's actions must also enable that component's tests. Preserve full
component coverage on master pushes. Keep CMake test-enable flags and action
conditions consistent so disabled targets are never built or executed.

Declare an explicit shell for each composite `run` step. Preserve test exit
codes, platform/engine restrictions, working directories, simulator selection,
and failure artifacts when moving or changing steps. Pass workflow values as
declared action inputs; composite actions do not inherit the caller's `inputs`
or `steps` contexts. Keep source-change detection in the calling workflow,
rather than duplicating it for every platform action.
