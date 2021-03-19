local bydict = {
	http_client = nil,
	command = "wget -q -O - %s",
	req_url = "https://cn.bing.com/dict/",
	match_pattern = '<div class="client_daily_word_en"><.->(%w-)</a>.-<div class="client_daily_word_zh">(.-)</div>',
}

function bydict:getreqcmd()
	return string.format(self.command, self.req_url)
end

--
--function bydict:start()
--	http_client = Http.new()
--	http_client:register(function(d, l)
--		self:onfetch(d, l)
--	end)
--	http_client:request(self.req_url)
--end

function bydict:onfetch(data, len)		
	local dict_en = nil
	local dict_zh = nil
    
	string.gsub(data, self.match_pattern, function(en,zh)
		dict_en = en
		dict_zh = zh
	end)
    
    print(dict_en, dict_zh)
	if dict_en ~= nil and dict_zh ~= nil then
		print(dict_en, dict_zh)

		local w = 300
		local h = 50

		local ml = ML.new("dict", 1920-w, 0, w, h)
		ml:show()

		local lb = Label.new(dict_en.." "..dict_zh, 16)
		ml:add(lb)
	end	
end

function bydict:start()	
	local cmd = self:getreqcmd()
	local pread = io.popen(cmd)
	local content = pread:read("*a")
	pread:close()	
	self:onfetch(content, string.len(content))
end

return bydict