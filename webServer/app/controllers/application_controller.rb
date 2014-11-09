class ApplicationController < ActionController::Base
  protect_from_forgery
  before_filter :authorize, :except=> [:login, :update, :new, :create, :closeApp]
  before_filter :vmStatus, :except=> [:login, :update, :new, :create, :closeApp]
  
  
  def vmStatus
    @vm_lists=User.find(session[:user_id]).vm_lists
  end
  
  def port
    11605
  end

  def controlServerIP
    '192.168.30.217'
  end    
  protected
  def authorize
    unless User.find_by_id(session[:user_id])
      session[:original_uri]=request.url
      flash[:notice]="Please, sign in first!"
      redirect_to :controller=>'main',:action=>'login'
    end
  end
end
