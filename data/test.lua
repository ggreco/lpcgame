math.randomseed(os.time())

-- load a map

Engine.load_map("outdoor.tmx")

-- load some characters
local hero = Character.new("hero.xml")
local sta = Character.new("student_a.xml", 100, 100)
local stb = Character.new("student_b.xml", 500, 100)
local bat1 = Character.new("bat.xml", 150, 150)
local bat2 = Character.new("bat.xml", 250, 250)
local bee = Character.new("bee.xml", 250, 250)

-- Set an hero and position it
Engine.set_hero(hero)
hero:set_position(400, 400)

-- Initialize engine counters, needed before the first .iteration
Engine.start()

local it = 0
bat2:goto(650,480)

repeat
    it = it + 1
    -- every ten cycles I check if the distance
    if it > 10 then
        it = 0

        if sta:moving() == false and sta:distance_from(hero) < 50 then
            sta:goto(sta:x() + math.random(-200, 200), 
                     sta:y() + math.random(-200, 200))
        end
        if stb:moving() == false and stb:distance_from(hero) < 50 then
            stb:goto(math.random(32, 600), math.random(32, 500))
        end

        if bee:moving() == false then
            bee:goto(bee:x() + math.random(-200, 200),
                     bee:y() + math.random(-200, 200))
        end
    end
until Engine.iteration() == false

