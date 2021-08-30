local http = require("tools/http")
local bitcoin = {
	font_size = 12,
	last_time = 0,
	need_start = false,
	delta_time = 0.05,
	
	req_url = "https://api.coincap.io/v2/assets/bitcoin",
}

function bitcoin:getdata(data)
    return string.sub(string.match(data, "priceUsd\":\"([%d.]+)"), 1, 8)
end

function bitcoin:onfetch(data, len)	   
    data = self:getdata(data) 
    print(data)
	if data ~= nil then	
		local len = string.len(data)
		local w = (len + 2) * self.font_size
		local h = self.font_size * 4
		local x = 0
		local y = 0
		
		if win.screencount() == 1 then
			local l,r,t,b = win.creenrect(0)		
			x = r - l - w - w / 2
			y = b - t - h - 20
		else
			local l1,r1,t1,b1 = win.creenrect(0)
			local l2,r2,t2,b2 = win.creenrect(1)
			x = r1 + r2 - l2 - w - w / 2
			y = b2 - t2 - h
		end	

		local ml = ML.new("coin", x, y, w, h)
		ml:show()

		local lb = Label.new(data, self.font_size)
		lb:color(0,64,128,255)
		ml:add(lb)
		
		local k = self:setpoint(tonumber(data))
		if k ~= nil then
			local klb = Label.new(tostring(k), self.font_size)
			klb:color(0,64,128,255)
			klb:setpos(0, self.font_size + 5)
			ml:add(klb)
		end
	end	
end

function bitcoin:start()	
	self.need_start = false
	self.last_time = os.time()

	local content = http:get(self.req_url)
	self:onfetch(content, string.len(content))
end

function bitcoin:restart()	
	ML.del("coin")
	self.need_start = true
	self.last_time = os.time()
end

function bitcoin:refresh()
	self:restart()
end

function bitcoin:update()
	if self.need_start then
		if os.time() - self.last_time > 5 then
			self.last_time = os.time()
			self:start()
		end		
	else 
		if os.time() - self.last_time > 300 then
			self.last_time = os.time()
			self:restart()
		end
	end	
end

local function catmull(p0, p1, p2, p3, t)
	local t2 = t * t;
	local t3 = t2 * t;
	return p1 + 0.5 * (t * (p2 - p0) +
			t2 * (2 * p0 - 5 * p1 + 4 * p2 - p3) +
			t3 * (-p0 + 3 * p1 - 3 * p2 + p3))
end

function bitcoin:setpoint(p)
	self.points = self.points or {}
	table.insert(self.points, p)
	if #self.points > 20 then
		table.remove(self.points, 1)
	end

	local num = #self.points
	local n = math.floor(num / 4)
	if n == 0 then
		return
	end
	local ma = {}
	local index = num
	for i = 1, 4 do
		local a = 0
		for j = 1, n do
			a = a + self.points[index]
			index = index - 1
		end
		a = a / n
		table.insert(ma, 1, a)
	end
	--print(ma[1], ma[2], ma[3], ma[4])

	local d1 = catmull(ma[1], ma[2], ma[3], ma[4], 0.9 + self.delta_time)
	local d2 = catmull(ma[1], ma[2], ma[3], ma[4], 0.9 - self.delta_time)
	return (d1 - d2) / (2 * self.delta_time)
end

return bitcoin



