------------------------------------------------------------------------------
-- user_logic.vhd - entity/architecture pair------------------------------------------------------------------------------
--
-- ***************************************************************************
-- ** Copyright (c) 1995-2007 Xilinx, Inc.  All rights reserved.            **
-- **                                                                       **
-- ** Xilinx, Inc.                                                          **
-- ** XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS"         **
-- ** AS A COURTESY TO YOU, SOLELY FOR USE IN DEVELOPING PROGRAMS AND       **
-- ** SOLUTIONS FOR XILINX DEVICES.  BY PROVIDING THIS DESIGN, CODE,        **
-- ** OR INFORMATION AS ONE POSSIBLE IMPLEMENTATION OF THIS FEATURE,        **
-- ** APPLICATION OR STANDARD, XILINX IS MAKING NO REPRESENTATION           **
-- ** THAT THIS IMPLEMENTATION IS FREE FROM ANY CLAIMS OF INFRINGEMENT,     **
-- ** AND YOU ARE RESPONSIBLE FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE      **
-- ** FOR YOUR IMPLEMENTATION.  XILINX EXPRESSLY DISCLAIMS ANY              **
-- ** WARRANTY WHATSOEVER WITH RESPECT TO THE ADEQUACY OF THE               **
-- ** IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OR        **
-- ** REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE FROM CLAIMS OF       **
-- ** INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS       **
-- ** FOR A PARTICULAR PURPOSE.                                             **
-- **                                                                       **
-- ***************************************************************************
--
------------------------------------------------------------------------------
-- Filename:          user_logic.vhd
-- Version:           1.00.a
-- Description:       User logic.
-- Date:              Fri Nov 12 10:32:15 2010 (by Create and Import Peripheral Wizard)
-- VHDL Standard:     VHDL'93
------------------------------------------------------------------------------
-- Naming Conventions:
--   active low signals:                    "*_n"
--   clock signals:                         "clk", "clk_div#", "clk_#x"
--   reset signals:                         "rst", "rst_n"
--   generics:                              "C_*"
--   user defined types:                    "*_TYPE"
--   state machine next state:              "*_ns"
--   state machine current state:           "*_cs"
--   combinatorial signals:                 "*_com"
--   pipelined or register delay signals:   "*_d#"
--   counter signals:                       "*cnt*"
--   clock enable signals:                  "*_ce"
--   internal version of output port:       "*_i"
--   device pins:                           "*_pin"
--   ports:                                 "- Names begin with Uppercase"
--   processes:                             "*_PROCESS"
--   component instantiations:              "<ENTITY_>I_<#|FUNC>"
------------------------------------------------------------------------------

-- DO NOT EDIT BELOW THIS LINE --------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
--use ieee.std_logic_arith.all;
--use ieee.std_logic_unsigned.all;


library proc_common_v2_00_a;
use proc_common_v2_00_a.proc_common_pkg.all;
-- DO NOT EDIT ABOVE THIS LINE --------------------

--USER libraries added here

------------------------------------------------------------------------------
-- Entity section
------------------------------------------------------------------------------
-- Definition of Generics:
--   C_DWIDTH                     -- User logic data bus width
--   C_NUM_CE                     -- User logic chip enable bus width
--   C_IP_INTR_NUM                -- User logic number of interrupt event
--
-- Definition of Ports:
--   Bus2IP_Clk                   -- Bus to IP clock
--   Bus2IP_Reset                 -- Bus to IP reset
--   IP2Bus_IntrEvent             -- IP to Bus interrupt event
--   Bus2IP_Data                  -- Bus to IP data bus for user logic
--   Bus2IP_BE                    -- Bus to IP byte enables for user logic
--   Bus2IP_RdCE                  -- Bus to IP read chip enable for user logic
--   Bus2IP_WrCE                  -- Bus to IP write chip enable for user logic
--   IP2Bus_Data                  -- IP to Bus data bus for user logic
--   IP2Bus_Ack                   -- IP to Bus acknowledgement
--   IP2Bus_Retry                 -- IP to Bus retry response
--   IP2Bus_Error                 -- IP to Bus error response
--   IP2Bus_ToutSup               -- IP to Bus timeout suppress
------------------------------------------------------------------------------

entity user_logic is
  generic
  (
    -- ADD USER GENERICS BELOW THIS LINE ---------------
		C_NBUTTONS                    : integer               := 5;
    -- ADD USER GENERICS ABOVE THIS LINE ---------------



    -- DO NOT EDIT BELOW THIS LINE ---------------------
    -- Bus protocol parameters, do not add to or delete
    C_DWIDTH                       : integer              := 32;
    C_NUM_CE                       : integer              := 3;
    C_IP_INTR_NUM                  : integer              := 1
    -- DO NOT EDIT ABOVE THIS LINE ---------------------
  );
  port
  (
    -- ADD USER PORTS BELOW THIS LINE ------------------
    	Ext_Buttons   : in  std_logic_vector(0 to C_NBUTTONS-1);
		Ext_ssdan :out std_logic_vector (0 to 3);
		Ext_ssd: out std_logic_vector (0 to 7);
		Ext_LDG :out std_logic;
		Ext_LED: out std_logic_vector (0 to 7);
    -- ADD USER PORTS ABOVE THIS LINE ------------------

    -- DO NOT EDIT BELOW THIS LINE ---------------------
    -- Bus protocol ports, do not add to or delete
    Bus2IP_Clk                     : in  std_logic;
    Bus2IP_Reset                   : in  std_logic;
    IP2Bus_IntrEvent               : out std_logic_vector(0 to C_IP_INTR_NUM-1);
    Bus2IP_Data                    : in  std_logic_vector(0 to C_DWIDTH-1);
    Bus2IP_BE                      : in  std_logic_vector(0 to C_DWIDTH/8-1);
    Bus2IP_RdCE                    : in  std_logic_vector(0 to C_NUM_CE-1);
    Bus2IP_WrCE                    : in  std_logic_vector(0 to C_NUM_CE-1);
    IP2Bus_Data                    : out std_logic_vector(0 to C_DWIDTH-1);
    IP2Bus_Ack                     : out std_logic;
    IP2Bus_Retry                   : out std_logic;
    IP2Bus_Error                   : out std_logic;
    IP2Bus_ToutSup                 : out std_logic
    -- DO NOT EDIT ABOVE THIS LINE ---------------------
  );
end entity user_logic;

------------------------------------------------------------------------------
-- Architecture section
------------------------------------------------------------------------------

architecture IMP of user_logic is

	--declaration des modules a instancier
	component decodeur4bitsDec
	port ( twoscomp: in STD_LOGIC_VECTOR(4 downto 0);
		 ssdBits : out STD_LOGIC_VECTOR(7 downto 0));
	end component;

	component ssdDriver
	port(clk : in STD_LOGIC;
		digit3 : in STD_LOGIC_VECTOR(7 downto 0);
		digit2 : in STD_LOGIC_VECTOR(7 downto 0);
		digit1 : in STD_LOGIC_VECTOR(7 downto 0);
		digit0 : in STD_LOGIC_VECTOR(7 downto 0);
		ssd : out STD_LOGIC_VECTOR(7 downto 0);
		ssdan : out STD_LOGIC_VECTOR(3 downto 0)
	);
	end component;

  --USER signal declarations added here, as needed for user logic
	signal pushed   : std_logic;
  	signal chiffre1, chiffre2, chiffre3, chiffre4 : STD_LOGIC_VECTOR(4 downto 0);
	signal bits1, bits2, bits3, bits4 : STD_LOGIC_VECTOR(7 downto 0);
	signal ssdan : STD_LOGIC_VECTOR(3 downto 0);
	signal ssd : STD_LOGIC_VECTOR(7 downto 0);	
	signal data_unsigned : unsigned(15 downto 0);
	signal data_leds : unsigned(15 downto 0);
	--temps
	signal Buttons_tmp1  :   std_logic_vector(0 to C_NBUTTONS-1);
	signal Buttons_tmp2   :   std_logic_vector(0 to C_NBUTTONS-1);

  ------------------------------------------
  -- Signals for user logic slave model s/w accessible register example
  ------------------------------------------
  signal slv_reg0                       : std_logic_vector(0 to C_DWIDTH-1);--btn
  signal slv_reg1                       : std_logic_vector(0 to C_DWIDTH-1);--leds
  signal slv_reg2                       : std_logic_vector(0 to C_DWIDTH-1);--7seg
  signal slv_reg_write_select           : std_logic_vector(0 to 2);
  signal slv_reg_read_select            : std_logic_vector(0 to 2);
  signal slv_ip2bus_data                : std_logic_vector(0 to C_DWIDTH-1);
  signal slv_read_ack                   : std_logic;
  signal slv_write_ack                  : std_logic;

  ------------------------------------------
  -- Signals for user logic interrupt example
  ------------------------------------------
  signal interrupt                      : std_logic_vector(0 to C_IP_INTR_NUM-1);

begin

  --USER logic implementation added here

  ------------------------------------------
  -- Example code to read/write user logic slave model s/w accessible registers
  -- 
  -- Note:
  -- The example code presented here is to show you one way of reading/writing
  -- software accessible registers implemented in the user logic slave model.
  -- Each bit of the Bus2IP_WrCE/Bus2IP_RdCE signals is configured to correspond
  -- to one software accessible register by the top level template. For example,
  -- if you have four 32 bit software accessible registers in the user logic, you
  -- are basically operating on the following memory mapped registers:
  -- 
  --    Bus2IP_WrCE or   Memory Mapped
  --       Bus2IP_RdCE   Register
  --            "1000"   C_BASEADDR + 0x0
  --            "0100"   C_BASEADDR + 0x4
  --            "0010"   C_BASEADDR + 0x8
  --            "0001"   C_BASEADDR + 0xC
  -- 
  ------------------------------------------
  slv_reg_write_select <= Bus2IP_WrCE(0 to 2);
  slv_reg_read_select  <= Bus2IP_RdCE(0 to 2);
  slv_write_ack        <= Bus2IP_WrCE(0) or Bus2IP_WrCE(1) or Bus2IP_WrCE(2);
  slv_read_ack         <= Bus2IP_RdCE(0) or Bus2IP_RdCE(1) or Bus2IP_RdCE(2);


-- instantiation des quatre modules decodeur4bits et ssd driver
	d1 	: decodeur4bitsDec port map (chiffre1, bits1);
	d2 	: decodeur4bitsDec port map (chiffre2, bits2);
	d3	: decodeur4bitsDec port map (chiffre3, bits3);
	d4 	: decodeur4bitsDec port map (chiffre4, bits4);  
   
	ssd1 : ssdDriver port map(Bus2IP_Clk, bits1, bits2, bits3, bits4, ssd, ssdan );

	--affichage des leds
	Ext_LDG <='1';
	data_leds <= unsigned(slv_reg1(0 to C_DWIDTH-1));
	
	--copier les 16 bits constituant le nombre a afficher dans une variable de type signed
	data_unsigned <= unsigned(slv_reg2(0 to C_DWIDTH-1));
	

	
process(Bus2IP_Clk) is
	variable leds: std_logic_vector (0 to 7);
	begin
		if(data_leds) > 7 then 
			leds := "11111111";
		elsif(data_leds) = 7 then
			leds := "11111110";
		elsif(data_leds) = 6 then
			leds := "11111100";
		elsif(data_leds) = 5 then
			leds := "11111000";	
		elsif(data_leds) = 4 then
			leds := "11110000";			
		elsif(data_leds) = 3 then
			leds := "11100000";
		elsif(data_leds) = 2 then
			leds := "11000000";
		elsif(data_leds) = 1 then
			leds := "10000000";
		elsif(data_leds) = 0 then
			leds := "00000000";
		end if;	
		Ext_LED <= leds;
end process;

CALCUL_CHIFFRES : process (Bus2IP_Clk) is
	variable dizaine_min, unite_min, dizaine_sec, unite_sec, nombre: integer;
	begin
		if (data_unsigned > 5999 or data_unsigned < 0) then--affichage de "----"
			chiffre1 <= "11111";  
			chiffre2 <= "11111";
			chiffre3 <= "11111";
			chiffre4 <= "11111";
		else
			nombre := to_integer(data_unsigned);

			--calcul des dizaines de min
			dizaine_min := 0;
			for j in 0 to 9 loop
				if (nombre >= 600) then
					nombre:= nombre - 600;
					dizaine_min := dizaine_min + 1;
				end if;
			end loop;
			--calcul des unites de min
			unite_min := 0;
			for j in 0 to 9 loop
				if (nombre >= 60) then
					nombre:= nombre - 60;
					unite_min := unite_min + 1;
				end if;
			end loop;
			--calcul des dizaines de sec
			dizaine_sec := 0;
			for j in 0 to 6 loop
				if (nombre >= 10) then
					nombre:= nombre - 10;
					dizaine_sec := dizaine_sec + 1;
				end if;
			end loop;
			unite_sec :=nombre;	 
			
			--ajout d'un bit apres les minutes
			unite_min := unite_min + 16;

			--assignation des 4 chiffres de la minuterie
			chiffre1 <= std_logic_vector(to_unsigned(unite_sec,5));
			chiffre2 <= std_logic_vector(to_unsigned(dizaine_sec,5));   
			chiffre3 <= std_logic_vector(to_unsigned(unite_min,5));
			chiffre4 <= std_logic_vector(to_unsigned(dizaine_min,5));
		end if;
end process ;	
		
		-- envoi des vecteurs aux sorties
		Ext_ssdan <= ssdan;
		Ext_ssd <= ssd;
	
  -- implement slave model register(s)
  -- Also manages the button values
SLAVE_REG_WRITE_PROC : process( Bus2IP_Clk ) is
  begin
    if ( Bus2IP_Clk'event and Bus2IP_Clk = '1' ) then
	   if (slv_reg0(0 to C_NBUTTONS-1) /= Ext_Buttons) then
			  pushed <= '1';
			else
			  pushed <= '0';
	    end if;
		 
		 -- Move temporaries
		Buttons_tmp1 <= Ext_Buttons;
		Buttons_tmp2 <= Buttons_tmp1;
	end if;
				
    if Bus2IP_Clk'event and Bus2IP_Clk = '1' then
      if Bus2IP_Reset = '1' then
        slv_reg0 <= (others => '0');
        slv_reg1 <= (others => '0');
        slv_reg2 <= (others => '0');
      else
			-- If not resetting, move the temporary to reg0
			slv_reg0(0 to C_NBUTTONS-1) <= Buttons_tmp2;
        case slv_reg_write_select is
          when "100" =>
            for byte_index in 0 to (C_DWIDTH/8)-1 loop
              if ( Bus2IP_BE(byte_index) = '1' ) then
                slv_reg0(byte_index*8 to byte_index*8+7) <= Bus2IP_Data(byte_index*8 to byte_index*8+7);
              end if;
            end loop;
          when "010" =>
            for byte_index in 0 to (C_DWIDTH/8)-1 loop
              if ( Bus2IP_BE(byte_index) = '1' ) then
                slv_reg1(byte_index*8 to byte_index*8+7) <= Bus2IP_Data(byte_index*8 to byte_index*8+7);
              end if;
            end loop;
          when "001" =>
            for byte_index in 0 to (C_DWIDTH/8)-1 loop
              if ( Bus2IP_BE(byte_index) = '1' ) then
                slv_reg2(byte_index*8 to byte_index*8+7) <= Bus2IP_Data(byte_index*8 to byte_index*8+7);
              end if;
            end loop;
          when others => null;
        end case;
      end if;
	end if;

  end process SLAVE_REG_WRITE_PROC;

  -- implement slave model register read mux
  SLAVE_REG_READ_PROC : process( slv_reg_read_select, slv_reg0, slv_reg1, slv_reg2 ) is
  begin

    case slv_reg_read_select is
      when "100" => slv_ip2bus_data <= slv_reg0;
      when "010" => slv_ip2bus_data <= slv_reg1;
      when "001" => slv_ip2bus_data <= slv_reg2;
      when others => slv_ip2bus_data <= (others => '0');
    end case;

  end process SLAVE_REG_READ_PROC;

  ------------------------------------------
  -- Example code to generate user logic interrupts
  -- 
  -- Note:
  -- The example code presented here is to show you one way of generating
  -- interrupts from the user logic. This code snippet infers a counter
  -- and generate the interrupts whenever the counter rollover (the counter
  -- will rollover ~21 sec @50Mhz).
  ------------------------------------------
  INTR_PROC : process( Bus2IP_Clk ) is
  begin
    if ( Bus2IP_Clk'event and Bus2IP_Clk = '1' ) then
      if ( Bus2IP_Reset = '1' ) then
        interrupt <= (others => '0');
      else
        if ( pushed = '1' ) then
          interrupt <= (others => '1');
        else
          interrupt <= (others => '0');
        end if;
      end if;
    end if;

  end process INTR_PROC;

  IP2Bus_IntrEvent <= interrupt;

  ------------------------------------------
  -- Example code to drive IP to Bus signals
  ------------------------------------------
  IP2Bus_Data        <= slv_ip2bus_data;

  IP2Bus_Ack         <= slv_write_ack or slv_read_ack;
  IP2Bus_Error       <= '0';
  IP2Bus_Retry       <= '0';
  IP2Bus_ToutSup     <= '0';

end IMP;
