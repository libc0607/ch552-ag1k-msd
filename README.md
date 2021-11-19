# ch552-ag1k-msd
Load bitstream to AG1K series FPGA using CH552

AG1K does not have a built-in data flash;  
This design only simulate a USB mass storage drive;   
Drag the bitstream file into MSD, the CH552 loads the bitstream into the internal SRAM of AG1K.

# Compile 
 - Install Arduino IDE, then install [DeqingSun/ch55xduino](https://github.com/DeqingSun/ch55xduino)
 - Open this project, choose "CH552 Boards", "USER CODE w/ 148B USB ram", "16MHz 3.3V"
 - Upload

# Run 
 - Compile your design <design_name> in [Supra](http://www.alta-gate.com/);
 - Rename <design_name>.bin to SRAM.BIN (should be 46,808 bytes for AG1K; keep filename UPPERCASE);
 - Plug your CH552 board to your PC's USB, then drag SRAM.BIN into USB Mass Storage <AG1K MSD>, choose overwrite;
 - When loading finished, pin CDONE will be released (HIGH) by AG1K.

# Reference schematic
![Schematic_2021-11-19.png](https://github.com/libc0607/ch552-ag1k-msd/blob/main/pic/Schematic_2021-11-19.png?raw=true)  
This sch. is only used as an example to describe the connection related to this project, for reference only;  
Not include other circuits required for normal design (clock, reset, power, etc.);  
Pins can be modified except the hardware SPI pins; 
Please do check before use.  
  
# Reference Documents
zh-cn only. Contact AGM for other languages:)  
  
[AG1K_Rev1_0.pdf](https://github.com/libc0607/ch552-ag1k-msd/blob/main/refdocs/AG1K_Rev1_0.pdf)  
[MANUAL_AG1KLP.pdf](https://github.com/libc0607/ch552-ag1k-msd/blob/main/refdocs/MANUAL_AG1KLP.pdf)  
  
# License 
GNU Lesser General Public License v2.1  (same as [DeqingSun/ch55xduino](https://github.com/DeqingSun/ch55xduino))
  
---
  
# 还有简体中文版 README

用 CH552 给 AG1K FPGA 加载比特流  
AG1K 没有内置存储器，但是可以通过 SPI 往内置 SRAM 写比特流；所以这个烧写器只是通过模拟 U 盘拖拽烧录的方式工作   
（至于为什么不照抄 iCESugar 那种通过跳线选择的是因为……去对比一下这个芯片和拉铁丝的那个的主从模式下的引脚定义就知道了orz  

# 编译
 - 安装 Arduino IDE，然后去安装 [DeqingSun/ch55xduino](https://github.com/DeqingSun/ch55xduino)  
 - 下载并打开这个工程，设置板子为 "CH552 Boards", "USER CODE w/ 148B USB ram", "16MHz 3.3V" 
 - 上传

# 运行
 - 在 [Supra](http://www.alta-gate.com/) 中综合并生成比特流，工程名假设为 <design_name>；
 - 重命名工程目录下生成的 <design_name>.bin 为 SRAM.BIN（AG1K 应该有 46,808 字节，注意大写）；
 - 把 CH552 插入电脑 USB，把 SRAM.BIN 直接丢到 <AG1K MSD> 这个模拟出的 U 盘里覆盖；
 - 下载完成后 CDONE 会被释放
  
# 原理图参考
![Schematic_2021-11-19.png](https://github.com/libc0607/ch552-ag1k-msd/blob/main/pic/Schematic_2021-11-19.png?raw=true)  
本图只作为描述和本项目相关电路的连接关系的示例，仅供参考；  
不包含正常工作所需的其他电路（如时钟复位电源等）；  
除了硬件 SPI 相关引脚之外都可以改；  
请自行检查验证后再使用，产生的一切问题和我无关。  
  
# 参考文档
中文版也不好找就是了（  
[AG1K_Rev1_0.pdf](https://github.com/libc0607/ch552-ag1k-msd/blob/main/refdocs/AG1K_Rev1_0.pdf)  
[MANUAL_AG1KLP.pdf](https://github.com/libc0607/ch552-ag1k-msd/blob/main/refdocs/MANUAL_AG1KLP.pdf)  


  
# 协议 
GNU Lesser General Public License v2.1  （和 [DeqingSun/ch55xduino](https://github.com/DeqingSun/ch55xduino) 相同）
