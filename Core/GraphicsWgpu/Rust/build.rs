use std::env;

fn main() {
    println!("cargo:rerun-if-env-changed=BABYLON_WGPU_NATIVE_LIB_DIR");
    println!("cargo:rerun-if-env-changed=BABYLON_WGPU_NATIVE_LIB_NAME");

    if env::var_os("CARGO_FEATURE_UPSTREAM_WGPU_NATIVE").is_none() {
        return;
    }

    let lib_dir = env::var("BABYLON_WGPU_NATIVE_LIB_DIR").expect(
        "upstream_wgpu_native feature requires BABYLON_WGPU_NATIVE_LIB_DIR to be set by CMake",
    );
    let lib_name =
        env::var("BABYLON_WGPU_NATIVE_LIB_NAME").unwrap_or_else(|_| String::from("wgpu_native"));

    println!("cargo:rustc-link-search=native={lib_dir}");
    println!("cargo:rustc-link-lib=static={lib_name}");
}
