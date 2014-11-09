# encoding: UTF-8
class UsersController < ApplicationController
  before_filter :check_user, :only=>[:edit, :update, :show]

  def new
    @user = User.new
  end

  def show
    @user = User.find(params[:id])
  end

  def edit
    @user = User.find(params[:id])    
  end


  def create
    @user = User.new(params[:user])

    respond_to do |format|
      if @user.save
        format.html { redirect_to root_path, notice: 'User was created successfully.' }
      else
        format.html { render action: "new" }
      end
    end
  end

  def update
    @user = User.find(params[:id])
    
    @user.update_attributes!(params[:adm_user])
    if !@user.verified
      @user.verified=true
    end
    @user.save!
    redirect_to @user, notice: '成功編輯!'
    
    rescue ActiveRecord::RecordInvalid
      @user.valid? 
      render :action=>:edit
  end
  
protected
   
   def check_user
      if params[:id]!=session[:user_id].to_s        
        redirect_to :controller=>'main', :action=>'index', :notice=>'您沒有權限'
      end
   end     
end
