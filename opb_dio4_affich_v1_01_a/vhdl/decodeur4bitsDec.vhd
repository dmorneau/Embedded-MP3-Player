---------------------------------------------------------------------------------------------------
-- decodeur4bitsDec.vhd
--
-- Pierre Langlois
-- v. 1.0, 2004/07/23
--
-- Décodage - de complément à deux (5 bits) vers affichage à 7 segments.
-- Le point allumé indique un nombre négatif.
-- On a donc de -9 à +9 et le tiret (-).
--
---------------------------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.all;

entity decodeur4bitsDec is
	 port(
		 twoscomp : in STD_LOGIC_VECTOR(4 downto 0);
		 ssdBits : out STD_LOGIC_VECTOR(7 downto 0)
	     );
end decodeur4bitsDec;

architecture arch of decodeur4bitsDec is
begin

--
-- décodeur pour affichage à 7 segments (8 bits incluant le point)
-- correspondance entre bits et segments:
--      0
--     ---  
--  5 |   | 1
--     ---   <- 6
--  4 |   | 2
--     ---
--      3	 
--  point: bit 7
--

    -- JC: modifications pour avoir une notation
	--     avec et sens le point.
	with twoscomp(4 downto 0) select
	ssdBits <=
	   "11000000" when "00000", --  0
		"11111001" when "00001",	-- 1
		"10100100" when "00010",	-- 2
		"10110000" when "00011",	-- 3
		"10011001" when "00100",	-- 4
		"10010010" when "00101",	-- 5
		"10000010" when "00110",	-- 6 
		"11111000" when "00111",	-- 7
		"10000000" when "01000",	-- 8
		"10010000" when "01001",	-- 9
		
		"10111111" when "11111",	-- "-"
		
		"01111001" when "10001",	-- 1.
		"00100100" when "10010",	-- 2.
		"00110000" when "10011",	-- 3.
		"00011001" when "10100",	-- 4.
		"00010010" when "10101",	-- 5.
		"00000010" when "10110",	-- 6. 
		"01111000" when "10111",	-- 7.
		"00000000" when "11000",	-- 8.
		"00010000" when "11001",	-- 9.
		
		"01000000" when others;	--  0.	 (when "1000")
		
end arch;
