---------------------------------------------------------------------------------
-- Created by		: 
-- Filename			: tb_sqrt.vhd
-- Author			: ChenYong
-- Created On		: 2021-03-11 23:42
-- Last Modified	: 2021-03-11 23:42
-- Version			: v1.0
-- Description		: 
--						
--						
---------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;


entity tb_sqrt is
end tb_sqrt;

architecture behavior of tb_sqrt is
constant WIDTH : natural := 32;

	-- Clock period definitions
	constant clk_period : time := 64 ns;
     signal  x_s : STD_LOGIC_VECTOR (WIDTH-1 downto 0);
     signal  y_s : STD_LOGIC_VECTOR (WIDTH-1 downto 0);
     signal  clk_s :  STD_LOGIC;
     signal  reset_s :  STD_LOGIC;
     signal  start_s :  STD_LOGIC;
     signal  done_interrupt_s :  STD_LOGIC;
     signal  ready_s : STD_LOGIC;
begin 

	sqrt_inst : entity work.sqrt
	generic map (
		WIDTH			=> WIDTH
	)
	port map(
	       x_i => x_s,
           y_o => y_s,
           clk => clk_s,
           reset => reset_s,
           start => start_s,
           done_interrupt => done_interrupt_s,
           ready => ready_s	);
           
           
	-- Clock process definitions
	clkgen:process
	begin
		clk_s <= '0';
		wait for clk_period/2;
		clk_s <= '1';
		wait for clk_period/2;
	end process;

	-- Stimulus process
	process
	begin
		-- hold reset state for 100 ns
	   reset_s <= '0';
	   x_s <=  Std_logic_vector(to_Unsigned(16,WiDTH));
		wait for 100 ns;
		reset_s <= '1';
		start_s <= '1';
		wait for 100 ns;
        start_s <= '0';
		-- Add stimulus here

		wait;
	end process;

end behavior;

