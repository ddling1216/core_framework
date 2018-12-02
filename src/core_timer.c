#include "core_timer.h"

/* === 定时器 === */
void
TIMER_CB(EV_P_ ev_timer *timer, int revents){

    if (ev_have_watcher_userdata(timer)){

        lua_State *co = (lua_State *) ev_get_watcher_userdata(timer);

        int status = lua_resume(co, NULL, lua_gettop(co) > 0 ? lua_gettop(co) - 1 : 0);
        
        if (status != LUA_OK && status != LUA_YIELD){

        	LOG( "WARNNING", lua_tostring(co, -1));

        }
    }
}

int
timer_stop(lua_State *L){

	ev_timer *timer = (ev_timer *) luaL_testudata(L, 1, "__TIMER__");
	if(!timer) return 0;

	timer->repeat = 0.;

   	ev_timer_again (EV_LOOP_ timer);

	return 1;
}

int
timer_start(lua_State *L){

	ev_timer *timer = (ev_timer *) luaL_testudata(L, 1, "__TIMER__");
	if(!timer) return 0;

	lua_Number timeout = luaL_checknumber(L, 2);

	lua_Number repeat = luaL_checknumber(L, 3);

	lua_State *co = lua_tothread(L, 4);
	if(!co) return 0;

	ev_set_watcher_userdata(timer, (void*)co);

	timer->repeat = timeout ? timeout : repeat;

	ev_timer_again(EV_LOOP_ timer);

	lua_settop(L, 1);

	return 1;

}

int
timer_new(lua_State *L){

	ev_timer *timer = (ev_timer *) lua_newuserdata(L, sizeof(ev_timer));
	if(!timer) return 0;

	ev_init(timer, TIMER_CB);

	luaL_setmetatable(L, "__TIMER__");

	return 1;
	
}

int
luaopen_timer(lua_State *L){
    luaL_newmetatable(L, "__TIMER__");
    lua_pushstring (L, "__index");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);
    luaL_setfuncs(L, timer_libs,0);
    luaL_newlib(L, timer_libs);
    lua_setglobal(L, "core_timer");
    return 1;
}
