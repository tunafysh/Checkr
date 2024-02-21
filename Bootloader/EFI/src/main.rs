#![no_std]
#![no_main]
#![feature(abi_efiapi)]

use uefi::prelude::*;
use uefi::proto::console::gop::GraphicsOutput;
use uefi::table::boot::{AllocateType, MemoryType};
use uefi_services::init;

#[entry]
fn main(_handle: Handle, system_table: SystemTable<Boot>) -> Status {
    init(&system_table).expect_success("Failed to initialize utilities");

    let gop = system_table
        .boot_services()
        .locate_protocol::<GraphicsOutput>()
        .expect_success("Failed to get GOP");

    let gop = unsafe { &mut *gop.get() };

    let info = gop.current_mode_info();
    let resolution = info.resolution();

    let width = resolution.0;
    let height = resolution.1;

    // Simplified example, adjust as needed for actual centering
    let pos_x = width / 2;
    let pos_y = height / 2;

    // Here you would implement drawing "Hello, world!" at the calculated position.
    // This example does not include the drawing code, as UEFI drawing requires
    // more complex buffer management and font rendering.

    uefi_services::system_table().as_ref().console().write_line("Hello, world!");

    Status::SUCCESS
}