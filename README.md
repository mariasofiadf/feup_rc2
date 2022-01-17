#Reset Switch

#Reset Router


#Exp1

##tuxy3

ifconfig eth0 up
ifconfig eth0 172.16.Y0.1/24
ifconfig eth0

##tuxy4

ifconfig eth0 up
ifconfig eth0 172.16.Y0.254/24
ifconfig eth0

#Exp2

##tuxy2

ifconfig eth0 up
ifconfig eth0 172.16.Y1.1/24
ifconfig eth0

##configure vlans

in tuxy3???

in gkterm:

create vlan Y0
    >enable
    #configure terminal
    #vlan Y0
    #end
    #show vlan id Y0

add ports to vlan Y0

    Switch# configure terminal
    Enter configuration commands, one per line. End with CNTL/Z.
    Switch(config)# interface fastethernet 0/1
    Switch(config-if)# switchport mode access
    Switch(config-if)# switchport access vlan Y0
    Switch(config-if)# end
    Switch# show running config interface fastethernet 0/1
    Switch# show interfaces fastethernet 0/1 switchport


create vlan Y1
    >enable
    #configure terminal
    #vlan Y1
    #end
    #show vlan id Y1


##tuxy4

ifconfig eth1 up
ifconfig eth1 172.16.Y1.253/24
ifconfig eth1


#Enabling forwarding in tuxes

echo 1 > /proc/sys/net/ipv4/ip_forward

#Enabling echo reply to broadcast request

echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts


In tuxy3: # ip route add 172.16.Y1.0/24 via 172.16.Y0.254
or # route add -net 172.16.Y1.0/24 gw 172.16.Y0.254 
In tuxy2: # ip route add 172.16.Y0.0/24 via 172.16.Y1.253
or # route add -net 172.16.Y0.0/24 gw 172.16.Y1.253 

configure vlan for router's switch entry'