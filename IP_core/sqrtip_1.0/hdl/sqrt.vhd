library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;


entity sqrt is
    generic (WIDTH: integer := 32);
    Port ( x_i : in STD_LOGIC_VECTOR (WIDTH-1 downto 0);
           y_o : out STD_LOGIC_VECTOR (WIDTH-1 downto 0);
           clk : in STD_LOGIC;
           reset : in STD_LOGIC;
           start : in STD_LOGIC;
           done_interrupt : out STD_LOGIC;
           ready : out STD_LOGIC);
end sqrt;

architecture Behavioral of sqrt is
    type state_type is (idle,l0, l1, l2, l3);
    signal state, state_next: state_type;
    signal op, op_next: unsigned(WIDTH-1 downto 0);
    signal one, one_next: unsigned(WIDTH-1 downto 0);
    signal res, res_next: unsigned(WIDTH-1 downto 0);
begin
-- State and data registers
ff:process (clk, reset)
begin
    if reset = '0' then
        state <= idle;
        op <= (others => '0');
        one <= (others => '0');
        res <= (others => '0');
    elsif (rising_edge(clk)) then
        state <= state_next;
        op <= op_next;
        one <= one_next;
        res <= res_next;
     --   ready<='0';
    end if;
end process ff;
-- Combinatorial circuits
comb:process(op,one,res,op_next,one_next,res_next,start,state,state_next,x_i)
begin
-- Defaults:
    op_next<=op;
    one_next<=one;
    res_next<=res;    
    ready<='0';  
    done_interrupt<='0';  

case state is
    when idle =>
        ready <= '1';
        if (start = '1') then
            op_next <= unsigned(x_i);
            res_next<=(others=>'0');
            one_next<=to_unsigned(1,WIDTH);
            state_next <= l0;
	    
        else
            state_next <= idle;
        end if;

     when l0 =>
        one_next<=shift_left(unsigned(one),30);   
        state_next <= l1; 
 
     when l1=>
        one_next<=shift_right(unsigned(one),2);
        if(one_next > op)then
            state_next<=l1;
        else
            state_next<=l2;
        end if;

     when l2=>
        if(op>=(res+one))then
            op_next<=op-(res+one);
            res_next<=res+to_unsigned(2*to_integer(one),WIDTH);
        end if;
        state_next<=l3;
     
     when others=> 
        res_next<=shift_right(unsigned(res),1);
        one_next <= shift_right(unsigned(one),2);
        if(one_next /= to_unsigned(0,WIDTH))then
            state_next <= l2;
        else
            state_next <= idle;
            done_interrupt <= '1';
        end if; 
end case;     
end process comb;   

	--Output signal   
	y_o<=std_logic_vector(res); 
end Behavioral;