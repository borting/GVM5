class MainController < ApplicationController
  
  def index

  end
  
  def createVMjs
    if !params[:cpu].blank? and !params[:memory].blank? and params[:memory].to_i>=256 and params[:memory].to_i<=1024           
      index=1
      VmList.order('serial_no').all.each do |v|
        if v.serial_no==index
          index=index+1
        else
          break
        end    
      end   
      serial_no=index
      serial_no=serial_no.to_s.rjust(4, '0')                                                             # => '0005'    
      params[:cpu]=params[:cpu].to_s.rjust(2, '0')                                                       # => '05'
      params[:memory]=params[:memory].to_s.rjust(4, '0')                                                 # => '0005'
      s = TCPSocket.new controlServerIP, port
      sended_mg='0'+' '+'1'+' '+serial_no+' '+params[:cpu]+' '+params[:memory]
      s.write sended_mg     
      return_mg=s.recv(100)
      logger.info return_mg
      s.close
      result = return_mg.split(' ')
      if result[3]!='0.0.0.0'
        vm = VmList.new(:cpu=>params[:cpu], :memory=>params[:memory], :serial_no=>index, :available=>true, :ip=>result[3])
        user = User.find(session[:user_id])  
        vm.user=user
        vm.save! 
        render :json => {:serial_no => vm.serial_no, :cpu=>vm.cpu, :memory=>vm.memory, :success=>true }.to_json          
      else  
        render :json => {:success=>false, :reason=>'Cloud server created VM failed'  }.to_json                 
      end 
    else
      render :json => {:success=>false, :reason=>"Invalid input CPU or memory" }.to_json                 
    end                
  end  
  
  def shutDownVMjs
    vm_list=VmList.find_by_serial_no(params[:serial_no])
    serial_no=params[:serial_no]
    serial_no=serial_no.to_s.rjust(4, '0')    
    ip=vm_list.ip 
    s = TCPSocket.new controlServerIP, port
    sended_mg='0'+' '+'2'+' '+serial_no+' '+ip 
    s.write sended_mg     
    return_mg=s.recv(100)
    s.close 
    result = return_mg.split(' ')
    if result[3]!='0.0.0.0'
      vm_list.app_lists.clear
      vm_list.destroy
      render :json => {:serial_no => params[:serial_no], :success=>true }.to_json
    else
      render :json => {:serial_no => params[:serial_no], :success=>false }.to_json 
    end
  end  
  
  
  def restartVMjs
    vm_list=VmList.find_by_serial_no(params[:serial_no])
    serial_no=params[:serial_no]
    serial_no=serial_no.to_s.rjust(4, '0')    
    ip=vm_list.ip 
    s = TCPSocket.new controlServerIP, port
    sended_mg='0'+' '+'3'+' '+serial_no+' '+ip 
    s.write sended_mg     
    return_mg=s.recv(100)
    s.close
    result = return_mg.split(' ')
    if result[3]!='0.0.0.0'  
      vm_list.app_lists.clear        
      vm_list.ip=result[3]
      vm_list.save!
      render :json => {:serial_no => params[:serial_no], :success=>true }.to_json
    else
      render :json => {:serial_no => params[:serial_no], :success=>false }.to_json 
    end    
  end
  
  def openAppjs
     vm_list=VmList.find_by_serial_no(params[:serial_no])   
     serial_no=params[:serial_no]
     app_name =AppName.find(params[:app_name_id].to_i+1)
     app_abbr =app_name.abbr     
     serial_no=serial_no.to_s.rjust(4, '0')          
     ip=vm_list.ip 
     s = TCPSocket.new controlServerIP, port    
     sended_mg='0'+' '+'4'+' '+serial_no+' '+ip+' '+app_abbr 
     s.write sended_mg     
     return_mg=s.recv(100)
     s.close
     result = return_mg.split(' ')
     logger.info result[3]
     if result[3]!='0.0.0.0'
    	 app_list = AppList.new(:port=>result[5], :app_name_id=>params[:app_name_id])  
         app_list.vm_list=vm_list
         app_list.save!            
         render :json => {:serial_no => params[:serial_no], :app_name_id => params[:app_name_id], :success=>true, :port=>result[5],
													          :app_name=>app_name.name }.to_json
     else
        render :json => {:serial_no => params[:serial_no], :app_name_id => params[:app_name_id], :success=>false }.to_json 
     end    
  end

  def closeAppjs
     app_list=AppList.find_by_port(params[:port])    
     serial_no=app_list.vm_list.serial_no.to_s.rjust(4, '0')
     ip=app_list.vm_list.ip  
     s = TCPSocket.new controlServerIP, port    
     sended_mg='0'+' '+'6'+' '+serial_no+' '+ip+' '+app_list.port.to_s 
     logger.info sended_mg
     s.write sended_mg
     app_list.destroy
     render :json => {:success=>true }.to_json
  end

  
  def closeApp
    app_list=AppList.find_by_port(params[:port]) 
    app_list.destroy  
    logger.info request.remote_ip
    render :nothing => true
  end

  def checkAppjs
    app_list=AppList.find_by_port(params[:port]) 
    if app_list.nil?
     render :json => {:exist=>false}.to_json
    else
     render :json => {:exist=>true }.to_json
    end   
  end
  
  def getAppListjs
    vm_list=VmList.find_by_serial_no(params[:serial_no])   
    app_list_json=Array.new
    vm_list.app_lists.each do |a|
      app_list_json<<
      {
        :app_name_id  => a.app_name_id,
        :port => a.port       
      }
    end 
    render :json=>app_list_json.to_json       
  end
  
  def getAppTablejs
    app_table_json=Array.new
    AppName.all.each do |a|
      app_table_json<<
      {
        :name  => a.name    
      }
    end 
    render :json=>app_table_json.to_json     
  end
  
  def login
     session[:user_id]=nil
     if request.post?
      user=User.authenticate(params[:username],params[:password])
       if user
         session[:user_id]=user.id
         uri=session[:original_uri]
         session[:original_uri]=nil
         redirect_to ( uri|| {:controller=>'main', :action=>'index'} )
       else
         flash.now[:notice]="Worng username or password"
       end
     end
  end
  
  def logout
    session[:user_id]=nil
    flash[:notice]="Logout successful"
    redirect_to :controller=>"main" ,:action=>"index" 
  end  
  
  
end
