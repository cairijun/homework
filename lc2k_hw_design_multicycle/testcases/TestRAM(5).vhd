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
regs(0) <= "00000000100000010000000000110111";
regs(1) <= "00000000100001000000000000110011";
regs(2) <= "00000000100001110000000000101110";
regs(3) <= "00000001011001100000000000000000";
regs(4) <= "00000001100000000000000000000000";
regs(5) <= "00000000100001000000000000110000";
regs(6) <= "00000000001111000000000000000111";
regs(7) <= "00000000111111100000000000000010";
regs(8) <= "00000000111110010000000000000001";
regs(9) <= "00000001000000010000000000001100";
regs(10) <= "00000000100001000000000000101111";
regs(11) <= "00000000000011000000000000000001";
regs(12) <= "00000000100001000000000000110011";
regs(13) <= "00000001011001100000000000000000";
regs(14) <= "00000000000000110000000000000010";
regs(15) <= "00000000101110010000000000000001";
regs(16) <= "00000000100001000000000000110100";
regs(17) <= "00000001011001100000000000000000";
regs(18) <= "00000000101111100000000000000010";
regs(19) <= "00000000100001000000000000110010";
regs(20) <= "00000000001111000000000000000111";
regs(21) <= "00000001011101000000000000000000";
regs(22) <= "00000000100000110000000000110001";
regs(23) <= "00000000101111100000000000000010";
regs(24) <= "00000000100001000000000000110010";
regs(25) <= "00000000001111000000000000000111";
regs(26) <= "00000001011101000000000000000000";
regs(27) <= "00000000100001000000000000110000";
regs(28) <= "00000000001111000000000000000111";
regs(29) <= "00000000111111100000000000000010";
regs(30) <= "00000000111111010000000000000001";
regs(31) <= "00000000000000000000000000000011";
regs(32) <= "00000000100001010000000000110101";
regs(33) <= "00000000100001100000000000110110";
regs(34) <= "00000000010101010000000000000100";
regs(35) <= "00000001001001100000000000000001";
regs(36) <= "00000000000110010000000000000011";
regs(37) <= "00000000001011010000000000000101";
regs(38) <= "00000001001010000000000000000010";
regs(39) <= "00000000000010010000000000000001";
regs(40) <= "00000001000000001111111111111001";
regs(41) <= "00000000101111010000000000000001";
regs(42) <= "00000000101111100000000000000010";
regs(43) <= "00000000100001000000000000110010";
regs(44) <= "00000000001111000000000000000111";
regs(45) <= "00000001011101000000000000000000";
regs(46) <= "00000000000000000000000011111111";
regs(47) <= "11111111111111111111111111111111";
regs(48) <= "11111111111111111111111111111110";
regs(49) <= "00000000000000000000000000000001";
regs(50) <= "00000000000000000000000000000010";
regs(51) <= "00000000000000000000000000000101";
regs(52) <= "00000000000000000000000000011011";
regs(53) <= "00000000000000000000000000000001";
regs(54) <= "11111111111111111111111111111111";
regs(55) <= "00000000000000000000000000001010";
	
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
