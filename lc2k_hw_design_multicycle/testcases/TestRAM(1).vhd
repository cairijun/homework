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
regs(0) <= "00000000100000010000000000000111";
regs(1) <= "00000000100010100000000000000011";
regs(2) <= "00000000000010100000000000000001";
regs(3) <= "00000001000000010000000000000010";
regs(4) <= "00000001000000001111111111111101";
regs(5) <= "00000001110000000000000000000000";
regs(6) <= "00000001100000000000000000000000";
regs(7) <= "00000000000000000000000000000101";
regs(8) <= "11111111111111111111111111111111";
regs(9) <= "00000000000000000000000000000010";
	
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
