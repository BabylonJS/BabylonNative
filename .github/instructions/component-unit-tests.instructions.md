---
applyTo: ".github/workflows/**"
---

# Component platform unit tests

Put component-specific platform builds and unit tests in reusable workflows:

- `.github/workflows/urllib-{win32,linux,macos}.yml`
- `.github/workflows/jsruntimehost-{win32,linux,macos,android,ios}.yml`

Follow `.github/workflows/<component>-<platform>.yml` for additional components
or supported platforms. Declare `on: workflow_call`. GitHub does not discover
or call workflows in subfolders, so keep these files directly in
`.github/workflows`. Do not add empty workflows for unsupported test harnesses.

Keep triggers, source-change detection and job dependencies in `ci.yml`.
Invoke each test workflow with a job-level `uses`, `needs` listing the
corresponding platform build job(s) and source-change detector, and `if`
checking that component's output. Keep component test code out of the
`build-<platform>.yml` workflows and do not duplicate it in composite actions.

Reusable workflow jobs run on separate runners. Each test workflow must check
out the repository, install its own tools, configure its component and build
its tests; never assume `needs` shares files or environment with the build job.
Pass values as declared workflow inputs. Standalone JsRuntimeHost's executable
and target are named `UnitTests`, not the embedded `JsRuntimeHostUnitTests`.

PR checks gate the components independently: JsRuntimeHost changes enable
JsRuntimeHost tests, and UrlLib changes enable UrlLib tests. Changes to a
component's `<component>-*.yml` workflows also enable its tests. Master pushes
run both components. Disable embedded component tests in platform build
workflows to avoid duplication; preserve existing local and nightly coverage.

Add every component test job to `ci-complete`. Accept a skipped job only when
its own component-change output is false; a changed component's skipped,
failed or cancelled test job must fail the required check. Do not accept all
skipped jobs indiscriminately. Preserve test exit codes, engine/compiler
coverage, working directories, simulator selection and failure artifacts.
