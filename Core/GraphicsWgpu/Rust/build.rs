use std::env;
use std::path::PathBuf;
use std::process::Command;

fn main() {
    println!("cargo:rerun-if-env-changed=BABYLON_WGPU_NATIVE_LIB_DIR");
    println!("cargo:rerun-if-env-changed=BABYLON_WGPU_NATIVE_LIB_NAME");
    println!("cargo:rerun-if-env-changed=BABYLON_WGPU_NATIVE_FFI_DIR");
    println!("cargo:rerun-if-env-changed=BABYLON_WEBGPU_HEADERS_DIR");

    let lib_dir = env::var("BABYLON_WGPU_NATIVE_LIB_DIR").expect(
        "BABYLON_WGPU_NATIVE_LIB_DIR must be set by CMake",
    );
    let lib_name =
        env::var("BABYLON_WGPU_NATIVE_LIB_NAME").unwrap_or_else(|_| String::from("wgpu_native"));

    println!("cargo:rustc-link-search=native={lib_dir}");
    println!("cargo:rustc-link-lib=static={lib_name}");

    let ffi_dir = PathBuf::from(env::var("BABYLON_WGPU_NATIVE_FFI_DIR").expect(
        "BABYLON_WGPU_NATIVE_FFI_DIR must be set by CMake",
    ));
    let header = ffi_dir.join("wgpu.h");
    let webgpu_headers = env::var_os("BABYLON_WEBGPU_HEADERS_DIR")
        .map(PathBuf::from)
        .unwrap_or_else(|| ffi_dir.join("webgpu-headers"));
    let webgpu_header = webgpu_headers.join("webgpu.h");

    assert!(
        webgpu_header.exists(),
        "Unable to locate webgpu.h under {}",
        webgpu_headers.display()
    );

    println!("cargo:rerun-if-changed={}", header.display());
    println!("cargo:rerun-if-changed={}", webgpu_header.display());

    let mut builder = bindgen::Builder::default()
        .header(
            header
                .to_str()
                .expect("BABYLON_WGPU_NATIVE_FFI_DIR path was not valid UTF-8"),
        )
        .clang_arg(format!("-I{}", webgpu_headers.display()))
        .parse_callbacks(Box::new(bindgen::CargoCallbacks::new()))
        .allowlist_item("WGPU.*")
        .allowlist_item("wgpu.*")
        .prepend_enum_name(false)
        .derive_default(true)
        .size_t_is_usize(true)
        .clang_macro_fallback()
        .formatter(bindgen::Formatter::Prettyplease)
        .layout_tests(false);

    match env::var("CARGO_CFG_TARGET_OS").ok().as_deref() {
        Some("ios") => {
            let target_triple = env::var("TARGET").unwrap_or_default();
            let is_simulator = target_triple.contains("apple-ios-sim")
                || target_triple.contains("x86_64-apple-ios");
            if is_simulator {
                let sdk = xcrun_sdk_path("iphonesimulator");
                let clang_target = if target_triple.contains("x86_64-apple-ios") {
                    "x86_64-apple-ios-simulator"
                } else {
                    "arm64-apple-ios-simulator"
                };
                builder = builder
                    .clang_arg(format!("-isysroot{sdk}"))
                    .clang_arg(format!("--target={clang_target}"));
            } else {
                let sdk = xcrun_sdk_path("iphoneos");
                builder = builder
                    .clang_arg(format!("-isysroot{sdk}"))
                    .clang_arg("--target=arm64-apple-ios");
            }
        }
        Some("macos") => {
            let sdk = xcrun_sdk_path("macosx");
            builder = builder.clang_arg(format!("-isysroot{sdk}"));
        }
        _ => {}
    }

    let bindings = builder
        .generate()
        .expect("Unable to generate WebGPU C API bindings");
    let out_path = PathBuf::from(env::var("OUT_DIR").expect("OUT_DIR was not set"));
    bindings
        .write_to_file(out_path.join("webgpu_bindings.rs"))
        .expect("Couldn't write webgpu_bindings.rs");
}

fn xcrun_sdk_path(sdk_name: &str) -> String {
    let output = Command::new("xcrun")
        .args(["--sdk", sdk_name, "--show-sdk-path"])
        .output()
        .expect("xcrun failed while resolving Apple SDK path");
    if !output.status.success() {
        panic!("xcrun failed while resolving {sdk_name} SDK path");
    }

    String::from_utf8(output.stdout)
        .expect("xcrun emitted invalid UTF-8")
        .trim()
        .to_owned()
}
