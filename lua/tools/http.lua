local http = {}

local function getwgetcmd(url)
    local command = "wget -q -O - \"%s\""
	return string.format(command, url)
end

function http:get1(url)
    local cmd = getwgetcmd(url)
	local pread = io.popen(cmd)
	local content = pread:read("*a")
	pread:close()	
    return content
end

function http:get(url)
    return Http.get(url)
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


return http
