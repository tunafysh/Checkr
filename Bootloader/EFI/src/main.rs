#![no_main]
#![no_std]
use uefi::prelude::*;
use uefi::proto::console::gop::GraphicsOutput;

#[entry]
fn main(image: uefi::Handle, system_table: &mut SystemTable<Boot>) -> Status {
    let gop = system_table.boot_services().locate_protocol::<GraphicsOutput>().unwrap().unwrap();

    let (width, height) = gop.current_mode_info().resolution();

    let x = (width / 2) - 40; // 40 is an approximate width of "Checkmate."
    let y = height / 2; // approximate center

    let color = PixelColor::rgb(255, 255, 255); // white color
    let bg_color = PixelColor::rgb(0, 0, 0); // black color

    for i in x..x + 80 { // 80 is an approximate width of "Checkmate."
        for j in y-8..y + 16 { // 16 is an approximate height of "Checkmate."
            gop.blt(
                Pixel::new(i as u32, j as u32, color),
                &[(i as u32, j as u32, 1, 1)],
            )
        }
    }

    system_table.boot_services().stall(10_000_000);

    Status::SUCCESS
}