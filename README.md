# lxmlparse
基于 rapidxml 解析 xml 到 lua table

**不支持属性和子结点同名**

```
-- test.xml
<node1 attr="1" />
<node1 attr="2" />
<node2 attr="3" />

-- output
{
	["node1"] = {
		[1] = {
			["attr"] = "1",
		},
		[2] = {
			["attr"] = "2",
		},
	},
	["node2"] = {
		[1] = {
			["attr"] = "3",
		},
	},
}
```

```
-- test2.xml
<node attr="1">
  <node attr="2"/>
</node>

-- output
{
	["node"] = {
		[1] = {
			["attr"] = "1",
			["node"] = {
				[1] = {
					["attr"] = "2",
				},
			},
		},
	},
}
```