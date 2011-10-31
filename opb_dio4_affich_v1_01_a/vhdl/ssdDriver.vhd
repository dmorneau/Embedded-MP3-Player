---------------------------------------------------------------------------------------------------
-- ssdDriver.vhd
--
-- Pierre Langlois
-- v. 1.0, 2004/07/23
-- v. 1.1, 2007/05/31 mise à jour des commentaires
--
-- Digilent Spartan-3 Starter Board
-- Xilinx University Program Virtex-II Pro Board (expansion DIO4)
-- Driver pour les 4 affichages à 7 segments 
--
-- Il faut fournir une horloge à 50 MHz ou à 100 MHz.
-- Elle est divisée à l'interne par 2^12, ce qui donne un taux de rafraichissement de 12 ou 24 KHz.
--
---------------------------------------------------------------------------------------------------

library IEEE;		  
use ieee.std_logic_signed.all;
use IEEE.STD_LOGIC_1164.all;

entity ssdDriver is
	port(
		clk : in STD_LOGIC;
		digit3 : in STD_LOGIC_VECTOR(7 downto 0);
		digit2 : in STD_LOGIC_VECTOR(7 downto 0);
		digit1 : in STD_LOGIC_VECTOR(7 downto 0);
		digit0 : in STD_LOGIC_VECTOR(7 downto 0);
		ssd : out STD_LOGIC_VECTOR(7 downto 0);
		ssdan : out STD_LOGIC_VECTOR(3 downto 0)
	);
end ssdDriver;

architecture ssdDriver of ssdDriver is

signal clk_int : std_logic; -- horloge interne
signal compte : std_logic_vector(1 downto 0) := "00";

begin

	-- Diviseur d'horloge
	-- L'horloge de 50 ou 100 MHz est ramenée à environ 10 KHz,
	-- sinon les quatre chiffres se "fondent" l'un dans l'autre.
	process(clk)
	variable clkCount : std_logic_vector(11 downto 0) := (others => '0');
	begin
		if (clk'event and clk = '1') then
			clkCount := clkCount + '1';
		end if;
		clk_int <= clkCount(clkCount'left);
	end process;

	-- Compteur pour 4 phases, correspondant à chacun des 4 chiffres
	process(clk_int)
	begin
		if (clk_int'event and clk_int = '1') then
			compte <= compte + '1';
		end if;
	end process;
	
	-- Assignation des signaux aux anodes des 4 chiffres
	with compte select		
	ssdan <=
		"1110" when "00",
		"1101" when "01",
		"1011" when "10",
		"0111" when others;
		
	-- Assignation des signaux aux segments des 4 chiffres
	with compte select		
	ssd <=
		digit0 when "00",
		digit1 when "01",
		digit2 when "10",
		digit3 when others;
		
end ssdDriver;
