[<img src="http://www.centecnetworks.com/en/images/logo.png" width=200>][1]


##Overview
__Lantern__ is an open source implementation targeted at hardware based SDN switch, integrating Linux Debian 7.2 OS, Open vSwitch(OVS) with silicon SDK and adaption layer as an open source turnkey solution. 

SDN is a new approach to networking of which OpenFlow is one of the key technologies. Implementing SDN via an open standard enables extraordinary agility while reducing service development and operational costs. The complete open source release including silicon SDK and adaption layer facilitates research activity in the field. It also enables equipment vendors to further innovate on top of the turnkey solution. Furthermore, __Lantern__ release is intended to contribute to the open eco-system which is crucial to SDN development in the industry. 

__Lantern__ release include source code of Debian Linux as platform OS, modified Open vSwitch(OVS) as openflow agent, as well as silicon SDK and adaption layer. In conjunction with the release, it includes development guide and compilation scripts. __Lantern__ features:
* OpenFlow 1.0 interoperating with mainstream OpenFlow controllers
* 2.5K embedded flow entries with complete match field and stats
* Complete L2 to L4 match fields
* Flexible editing L2-L4 fields.such as rewriting MAC DA/MAC SA/l4port/IP
* Popular tunnel overlay technology such as L2 over GRE, QinQ

Combined with __Lantern__ release is a high-performance development platform, V330-52TX-RD, powered by Centec's SDN optimized switch silicon. V330-52TX-RD is a product level development platform with 48*1GE RJ45 network ports and 4*10GE SFP+ uplink ports. The development system can be purchased by E-commerce at Amazon.com.

##Main features
* Support Centec Openflow hardware switching system: V330 52TX-RD.
* Support Openflow protocol 1.0/1.3
* Support ovs tools: ovs-ofctl, ovs-vsctl, ovsdb-server, ovsdb-tool, ovs-appctl, ovs-pki etc.
* Support flow matching fields:  in\_port, dl\_vlan, dl\_vlan\_pcp, dl\_src, dl\_dst, dl\_type, nw\_src, nw\_dst, nw\_proto, nw\_tos, tp\_src, tp\_dst, icmp\_code, icmp\_type
* Support flow priority, cookie, idle\_timeout and hard\_timeout
* Support multiple actions: drop, output (physical port, all, in\_port, controller, gre tunnel port), mod\_vlan_id, push\_vlan, strip\_vlan, mod\_dl\_src, mod\_dl\_dst, set\_tunnel\_id, mod\_nw\_dst, mod\_tp\_dst
* Support GRE tunnel port
* Support boot from Debian Linux system (Debian Linux 7.2 is embedded) 


##System image and Open vSwitch package build guide
The Virtual Centec Operating System (VCOS) image and Open vSwitch package can be built with GNU toolchain.

Please refer to 'BUILD-GUIDE' for details. 


##License

| Package                             | Version                      | License  |
|:------------------------------------|:-----------------------------|:---------|
| Centec openflow adapter layer & SDK | 1.0                          | Apache 2 |
| Open vSwitch                        | 1.10.0                       | Apache 2 |
| Linux Kernel                        | 2.6.32.23                    | GPL      |
| Busybox                             | 1.6.1                        | GPL      |

Please refer to 'LICENSE' for details.

##About Centec Networks
Centec Networks is a technology leader providing high-speed carrier-grade IP/Ethernet switching silicon and advanced ODM/OEM system solutions. Since 2005, Centec has delivered a series of silicon (TransWarp Series) and system products, covering a wide range of carrier access and aggregation market.

Since the inception of the company, Centec has gradually formed a complete family of TransWarp Ethernet series silicon. In 2013, Centec launched the third-generation switching Silicon GreatBelt Series, integrates Layer 2 through Layer 4 packet-processing engine and advanced traffic manager.The GreatBelt series Ethernet silicon can widely be applied to Metro, Enterprise, NID, SDN, PTN/IP RAN, and so on. Also SDK and EADP are available for low cost and shorten the Time-to-Marketing of customer's products. In order to provide the maximum convenience and value to custom,Centec has been cooperated with leading PHY vendor LSI, to provide a competitive turnkey solution of switching device.

Leveraging Centec's TransWarp Family of Ethernet Silicon, Centec's E series switch perfectly support the plenty features of Metro-E to provide rich Ethernet services. Centec has been cooperating with leading PHY and CPU vendors to offer one-step solution and service for short Time-to-Marketing. What's more, V Series Switches, the leading industry OpenFlow switches, are integrated with the open source Open vSwitch and Centec's SDK to provide a complete system solution. Centec's cutting-edge silicon products and system solutions enable telecom/network equipment vendors to build complete product portfolios with dramatically reduced development costs and rapidly improved time-to-market.

##Contact us
   * Website: [http://www.centecnetworks.com] [1]
   * Wiki: [https://github.com/centecnetworks/lantern/wiki] [2]
   * Issue tracker: [https://github.com/centecnetworks/lantern/issues] [3]
   * Support email: <support@centecnetworks.com>
   * Sales email: <sales@centecnetworks.com>
   * Weibo: [http://weibo.com/centec] [4]

[1]: http://www.centecnetworks.com "Centec Networks Co., Ltd."
[2]: https://github.com/centecnetworks/lantern/wiki "Lantern Project Wiki"
[3]: https://github.com/centecnetworks/lantern/issues "Lantern Project Issues"
[4]: http://weibo.com/centec "Centec Weibo"
