local oneword = {
	font_size = 12,
	last_time = 0,
	need_start = false,
	http_client = nil,
	command = "wget -q -O - %s",
	req_url = "https://v1.hitokoto.cn/?c=f&encode=text",
}
function oneword:getreqcmd()
	return string.format(self.command, self.req_url)
end

function oneword:onfetch(data, len)	    
    print(data)
	if data ~= nil then	
		local len = string.len(data)
		local w = (len / 3 + 2) * self.font_size
		local h = self.font_size * 2

		local ml = ML.new("dict", 1920-w, 0, w, h)
		ml:show()

		local lb = Label.new(data, self.font_size)
		lb:color(0,64,32,255)
		ml:add(lb)
	end	
end

function oneword:fetchword()
	local cmd = self:getreqcmd()
	local pread = io.popen(cmd)
	local content = pread:read("*a")
	pread:close()	
	return content
end

function oneword:start()	
	self.need_start = false
	self.last_time = os.time()

	local content = self:fetchword()
	self:onfetch(content, string.len(content))
end
--[[
function oneword:start() 
    if self.http_client == nil then
        self.http_client = Http.new()
        self.http_client:register(function(d, l)
            self:onfetch(d, l)
        end)
    end
    self.http_client:request(self.req_url)
end
]]

function oneword:restart()
	print("next")
	ML.del("dict")
	self.need_start = true
	self.last_time = os.time()
end

function oneword:update()
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

return oneword