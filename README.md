[<img src="http://www.centecnetworks.com/en/images/logo.png" width=200>][1]


##Overview
The Centec Openflow switching system is the open source software package for Centec V330 52TX-RD switch. 

The Centec V330 Series Switch is based on Centec's TransWarp series switching silicon and the mature ToR switch hardware, integrated with the open source Open vSwitch and Centec's SDK to provide a complete system solution. Taking advantages of V330's friendly interface and Openness, users can easily develop the customized SDN (Software Defined Network) solutions to realize various networking visualization.

Leveraging Centec's high performance Ethernet switching silicon CTC6048, the solution provides 176Gbps wire-speed forwarding capability with abundant OpenFlow features. Currently, V330 provides 48x1GE RJ45/SFP network ports and 4x10GE SFP+ modular cards as uplinks. 

Visit us at [http://www.centecnetworks.com] [1] 


##Main features
* Support Centec Openflow hardware switching system: V330 52TX-RD.
* Support Openflow protocol 1.0/1.3
* Support ovs tools: ovs-ofctl, ovs-vsctl, ovsdb-server, ovsdb-tool, ovs-appctl, ovs-pki etc.
* Support flow matching fields:  in\_port, dl\_vlan, dl\_vlan\_pcp, dl\_src, dl\_dst, dl\_type, nw\_src, nw\_dst, nw\_proto, nw\_tos, tp\_src, tp\_dst, icmp\_code, icmp\_type
* Support flow priority, cookie, idle\_timeout and hard\_timeout
* Support multiple actions: drop, output (physical port, all, in\_port, controller, gre tunnel port), mod\_vlan_id, push\_vlan, strip\_vlan, mod\_dl\_src, mod\_dl\_dst, set\_tunnel\_id, mod\_nw\_dst, mod\_tp\_dst
* Support GRE tunnel port
* Support boot from Debian Linux system (Debian Linux 7.2 is embedded) 


##How to build the system image and Open vSwitch package
The Virtual Centec Operating System (VCOS) image and Open vSwitch package can be built with GNU toolchain.

Please refer to 'BUILD-GUIDE' for details. 


##License

| Package                       | Version                      | License  |
|:------------------------------|:-----------------------------|:---------|
| Centec openflow adapter layer | 1.0                          | Apache 2 |
| Centec Humber SDK             | 2.0.4                        | Apache 2 |
| Open vSwitch                  | 1.10.0                       | Apache 2 |
| Linux Kernel                  | 2.6.32.23                    | GPL      |
| Busybox                       | 1.6.1                        | GPL      |

Please refer to 'LICENSE' for details.

##About Centec Networks
Centec Networks is a technology leader providing high-speed carrier-grade IP/Ethernet switching silicon and advanced ODM/OEM system solutions. Since 2005, Centec has delivered a series of silicon (TransWarp Series) and system products, covering a wide range of carrier access and aggregation market.

Since the inception of the company, Centec has gradually formed a complete family of TransWarp Ethernet series silicon. In 2013, Centec launched the third-generation switching Silicon GreatBelt Series, integrates Layer 2 through Layer 4 packet-processing engine and advanced traffic manager.The GreatBelt series Ethernet silicon can widely be applied to Metro, Enterprise, NID, SDN, PTN/IP RAN, and so on. Also SDK and EADP are available for low cost and shorten the Time-to-Marketing of customer's products. In order to provide the maximum convenience and value to custom,Centec has been cooperated with leading PHY vendor LSI, to provide a competitive turnkey solution of switching device.

Leveraging Centec's TransWarp Family of Ethernet Silicon, Centec's E series switch perfectly support the plenty features of Metro-E to provide rich Ethernet services. Centec has been cooperating with leading PHY and CPU vendors to offer one-step solution and service for short Time-to-Marketing. What's more, V Series Switches, the leading industry OpenFlow switches, are integrated with the open source Open vSwitch and Centec's SDK to provide a complete system solution. Centec's cutting-edge silicon products and system solutions enable telecom/network equipment vendors to build complete product portfolios with dramatically reduced development costs and rapidly improved time-to-market.

##Contact us
   * Website: [http://www.centecnetworks.com] [1]
   * Wiki: [https://github/centecnetworks/xxx/wiki] [2]
   * Issue tracker: [https://github/centecnetworks/xxx/issues] [3]
   * Support email: <support@centecnetworks.com>
   * Sales email: <sales@centecnetworks.com>
   * Weibo: [http://weibo.com/centec] [4]

[1]: http://www.centecnetworks.com "Centec Networks Co., Ltd."
[2]: https://github/centecnetworks/xxx/wiki "Project Wiki"
[3]: https://github/centecnetworks/xxx/issues "Project Issues"
[4]: http://weibo.com/centec "Centec Weibo"
