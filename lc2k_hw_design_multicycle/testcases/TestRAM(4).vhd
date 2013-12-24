library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_unsigned.all;
use IEEE.std_logic_arith.all;

entity test_ram is
   port( clk       : in std_logic;
      rst: in std_logic;
		enable      : in std_logic;
      R_W         : in std_logic;
      addr        : in std_logic_vector (31 downto 0);
      data_in     : in std_logic_vector (31 downto 0);
      data_out    : out std_logic_vector (31 downto 0));
end test_ram;

architecture BHV of test_ram is

type memory is array (0 to 255) of std_logic_vector(31 downto 0); 

signal regs : memory ;


begin
   process(clk, enable, R_W, addr)
   -- process(enable, R_W, addr)
   begin
	if (rst='1') then
 -- xxxxxxxccc111222iiiiiiiiiiiiiiii
regs(0) <= "00000000100001110000000000010101";
regs(1) <= "00000000100000010000000000011010";
regs(2) <= "00000000100001010000000000011001";
regs(3) <= "00000000100000110000000000011000";
regs(4) <= "00000000100000100000000000011000";
regs(5) <= "00000000100001000000000000010110";
regs(6) <= "00000001011001100000000000000000";
regs(7) <= "00000001100000000000000000000000";
regs(8) <= "00000001000011010000000000001011";
regs(9) <= "00000000100001000000000000010111";
regs(10) <= "00000000001111000000000000000111";
regs(11) <= "00000000111111100000000000000001";
regs(12) <= "00000000000000110000000000000100";
regs(13) <= "00000000000110100000000000000011";
regs(14) <= "00000000000001000000000000000010";
regs(15) <= "00000000100001000000000000011000";
regs(16) <= "00000000001011000000000000000101";
regs(17) <= "00000000101111100000000000000001";
regs(18) <= "00000000001111000000000000000111";
regs(19) <= "00000001000000001111111111110100";
regs(20) <= "00000001011101000000000000000000";
regs(21) <= "00000000000000000000000011111111";
regs(22) <= "00000000000000000000000000001000";
regs(23) <= "11111111111111111111111111111111";
regs(24) <= "00000000000000000000000000000001";
regs(25) <= "00000000000000000000000000000010";
regs(26) <= "00000000000000000000000000010100";
	
   elsif (enable = '1') then
       if (R_W = '0') then
           data_out <= regs(conv_integer(addr));
       elsif (clk'event and clk = '1') then
           regs(conv_integer(addr)) <=  data_in;
       end if;
   else 
   end if;
--end if;
end process;
end BHV;
