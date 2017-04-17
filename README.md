[<img src="http://www.centecnetworks.com/en/images/logo.png" width=200>][1]


## Overview
__Lantern__ is an open source implementation targeted at hardware based SDN switch, integrating Linux Debian 7.2 OS, Open vSwitch(OVS) with silicon SDK and adaption layer as an open source turnkey solution. 

SDN is a new approach to networking of which OpenFlow is one of the key technologies. Implementing SDN via an open standard enables extraordinary agility while reducing service development and operational costs. The complete open source release including silicon SDK and adaption layer facilitates research activity in the field. It also enables equipment vendors to further innovate on top of the turnkey solution. Furthermore, Lantern release is intended to contribute to the open eco-system which is crucial to SDN development in the industry. 

Lantern release include source code of Debian Linux as platform OS, modified Open vSwitch(OVS) as openflow agent, as well as silicon SDK and adaption layer. In conjunction with the release, it includes development guide and compilation scripts. Lantern features:
* OpenFlow 1.0 interoperating with mainstream OpenFlow controllers
* 2.5K embedded flow entries with complete match field and stats
* Complete L2 to L4 match fields
* Flexible editing L2-L4 fields.such as rewriting MAC DA/MAC SA/l4port/IP
* Popular tunnel overlay technology such as L2 over GRE, QinQ

Combined with Lantern release is a high-performance development platform, V330-52TX-RD, powered by Centec's SDN optimized switch silicon. V330-52TX-RD is a product level development platform with 48*1GE RJ45 network ports and 4*10GE SFP+ uplink ports. The development system can be purchased by E-commerce at Amazon.com.

## Main features
* Support Centec Openflow hardware switching system: V330 52TX-RD.
* Support Openflow protocol 1.0/1.3
* Support ovs tools: ovs-ofctl, ovs-vsctl, ovsdb-server, ovsdb-tool, ovs-appctl, ovs-pki etc.
* Support flow matching fields:  in\_port, dl\_vlan, dl\_vlan\_pcp, dl\_src, dl\_dst, dl\_type, nw\_src, nw\_dst, nw\_proto, nw\_tos, tp\_src, tp\_dst, icmp\_code, icmp\_type
* Support flow priority, cookie, idle\_timeout and hard\_timeout
* Support multiple actions: drop, output (physical port, all, in\_port, controller, gre tunnel port), mod\_vlan_id, push\_vlan, strip\_vlan, mod\_dl\_src, mod\_dl\_dst, set\_tunnel\_id, mod\_nw\_dst, mod\_tp\_dst
* Support GRE tunnel port
* Support boot from Debian Linux system (Debian Linux 7.2 is embedded) 


## System image and Open vSwitch package
The Virtual Centec Operating System (VCOS) image and Open vSwitch package can be built with GNU toolchain.

Please refer to 'BUILD-GUIDE' for details. 


## License

| Package                             | Version                      | License  |
|:------------------------------------|:-----------------------------|:---------|
| Centec openflow adapter layer & SDK | 1.0                          | Apache 2 |
| Open vSwitch                        | 1.10.0                       | Apache 2 |
| Linux Kernel                        | 2.6.32.23                    | GPL      |
| Busybox                             | 1.6.1                        | GPL      |

Please refer to 'LICENSE' for details.

## About Centec Networks
__Centec Networks__ is pioneering to provide switching silicon and whitebox solution for SDN. We commit to enable SDN into networks of carrier, enterprise and data center. Leveraging the high performance open SDN architecture, Centec enables customer networks seamlessly migrate from traditional L2, L3 and MPLS/MPLS-TP architecture to the new SDN track. Together with the customer, Centec is redefining the switching network with optimized investment and more opening capability to create value for future and today.

Since the inception of the company, Centec has gradually formed a complete family of TransWarpTM Ethernet switching silicon. In 2013, Centec launched the SDN optimized switching Silicon GreatBelt Series which redefines cost, power and price for Ethernet Switching. To bring the top convenience and value to OEMs, Centec has been cooperated with leading PHY and CPU vendors, to provide a competitive total solution for switching device.

Built on TransWarpTM Family core silicon, Centec's V Series Switches, the leading industry SDN switches with OVS and Open SDK integrated, provide a complete SDN whitebox solution for Datacenter, Enterprise and Carriers applications. As a high performance open physical SDN switch, it brightens the way to migrate the traditional L2/L3/MPLS/MPLS-TP network to the new SDN blueprint.

## Contact us
   * Website: [http://www.centecnetworks.com] [1]
   * Wiki: [https://github.com/centecnetworks/lantern/wiki] [2]
   * Issue tracker: [https://github.com/centecnetworks/lantern/issues] [3]
   * Support email: <support@centecnetworks.com>
   * Sales email: <sales@centecnetworks.com>
   * Weibo: [http://weibo.com/centec] [4]
   * Mailing list: [https://lists.sourceforge.net/lists/listinfo/lantern-project-discuss] [5]

[1]: http://www.centecnetworks.com "Centec Networks Co., Ltd."
[2]: https://github.com/centecnetworks/lantern/wiki "Lantern Project Wiki"
[3]: https://github.com/centecnetworks/lantern/issues "Lantern Project Issues"
[4]: http://weibo.com/centec "Centec Weibo"
[5]: https://lists.sourceforge.net/lists/listinfo/lantern-project-discuss "Mailing list for Lantern project"
