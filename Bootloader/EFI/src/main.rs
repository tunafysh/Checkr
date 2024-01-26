#![no_main]
#![no_std]

use core::fmt::Write;

use uefi::prelude::*;
use uefi::proto::console::text::Color;
use uefi::proto::console::gop::{*, self};
use uefi::table::runtime::ResetType;
use uefi_services::println;


#[entry]
fn main(_image_handle: Handle, mut system_table: SystemTable<Boot>) -> Status {
    uefi_services::init(&mut system_table).unwrap();
    system_table.stdout().clear();
    system_table.stdout().enable_cursor(false);
    system_table.stdout().set_color(Color::White, Color::Black);
    system_table.stdout().write_str("test\n");
    
    
    // Extract the number of rows and columns
    // let (width, height) = gop::ModeInfo::resolution();

    system_table.boot_services().stall(10_000_000);
    // system_table.runtime_services().reset(ResetType::WARM, Status::SUCCESS, Option::None);
    Status::SUCCESS
}
