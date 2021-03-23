local http = require("tools/http")
local bitcoin = {
	font_size = 12,
	last_time = 0,
	need_start = false,
	
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
		local h = self.font_size * 2

		local ml = ML.new("coin", 1920 + 2048 - w, 1152-h, w, h)
		ml:show()

		local lb = Label.new(data, self.font_size)
		lb:color(0,64,128,255)
		ml:add(lb)
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

return bitcoin



